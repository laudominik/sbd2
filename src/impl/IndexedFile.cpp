#include "IndexedFile.h"

#include <cmath>

#include <util/Constants.h>

namespace sbd::impl {
    IndexedFile::IndexedFile(): index(constants::INDEX_FILE), data(constants::DATA_FILE_NAME) {
        clear();

        // full pages closest to this number \|
        // startOfOverflowArea = static_cast<size_t>(static_cast<double>(data.maxSize()) * (1.0-constants::OVERFLOW_RATIO));
        // startOfOverflowArea = currentOverflowEndIx;

        for(auto pageNumber = 0u; pageNumber < constants::INITIAL_PAGES_COUNT; pageNumber++){
            index.push_back(IndexRecord(constants::INCORRECT_RECORD_KEY, pageNumber));
            data.appendEmptyPage();
        }

        static const auto OVERFLOW_PAGES_AMOUNT = static_cast<size_t>(ceil(constants::INITIAL_PAGES_COUNT * constants::OVERFLOW_RATIO));
        for(auto pageNumber = 0u; pageNumber < OVERFLOW_PAGES_AMOUNT; pageNumber++){
            data.appendEmptyPage();
        }
        currentOverflowEndIx = constants::INITIAL_PAGES_COUNT * constants::DATA_RECORD_PER_PAGE;

        index.flushCachedPage();
        data.flushCachedPage();

    }

    void IndexedFile::clear() {
        std::fstream f1;
        f1.open(constants::INDEX_FILE, std::ios::out);
        f1<<"";
        f1.close();
        std::fstream f2;
        f2.open(constants::DATA_FILE_NAME, std::ios::out);
        f2<<"";
        f2.close();
        index.reset();
        data.reset();
    }

    void IndexedFile::insert(generic::key_t key, const std::string& value) {
        auto [posInIndex, posInData] = getPositionFromIndex(key);
        auto indexRecord = index.get(posInIndex);

        // assert (primary area not full)

        // handle empty page
        if(indexRecord.getKey() == constants::INCORRECT_RECORD_KEY){
            IndexRecord newIndexRecord{indexRecord};
            newIndexRecord.setKey(key);
            index.insert(posInIndex, newIndexRecord);

            DataRecord newDataRecord{key, constants::INCORRECT_RECORD_KEY, value};
            data.insert(posInData, newDataRecord);
            return;
        }

        // TODO: handle last page (so that it doesn't overlap with overflow area)

        // find suitable position for record
        auto posOnPage = 0u;
        bool smallest = true;
        for(auto i = 0u; i < constants::DATA_RECORD_PER_PAGE; i++){
            auto checkedRecord = data.get(i);
            if(checkedRecord.getKey() < key){
                smallest = false;
                posOnPage = i;
            }
            if(checkedRecord.getKey() == key){
                // LOG this case
                return;
            }
        }

        // explicit handling of corner case, where the smallest record will go behind the first record of the first page
        if(smallest){
            addToChain(key, value, posInData, posInIndex);
            indexRecord.setKey(key);
            index.insert(posInIndex, indexRecord);
            postInsertJob();
            return;
        }

        posOnPage++;
        if(posOnPage < constants::DATA_RECORD_PER_PAGE ){
            // put it in empty place
            if(data.get(posOnPage + posInData).getKey() == constants::INCORRECT_RECORD_KEY){
                data.insert(posOnPage + posInData, impl::DataRecord(key, constants::INCORRECT_RECORD_KEY, value));
            } else {
                // add to chain of posOnPage - 1
                addToChain(key, value, posInIndex, posOnPage - 1 + posInData);
            }
        } else {
            // add to chain of last element
            addToChain(key, value, posInIndex, posOnPage - 1 + posInData);
        }

        postInsertJob();
    }

    // gets index of first record of suitable page in DATA FILE and index of the corresponding record in INDEX FILE
    std::pair<size_t, size_t> IndexedFile::getPositionFromIndex(generic::key_t key) {
        size_t posInIndex{}, posInData{};
        for(auto i = 0u; i < index.size(); i++){
            auto checkedRecord = index.get(i);
            if(checkedRecord.getKey() < key){
                posInIndex = i;
                posInData = checkedRecord.getPtr() * constants::DATA_RECORD_PER_PAGE;
            }
        }
        return {posInIndex, posInData};
    }

    // add (key, value) to chain of record with index = dataIx
    void IndexedFile::addToChain(generic::key_t key, std::string value, size_t indexIx, size_t dataIx) {
        while(true){
            auto checkedRecord = data.get(dataIx);

            if(checkedRecord.getKey() == key){
                // LOG this case
                return;
            }

            // keep the order ascending
            if(checkedRecord.getKey() > key){
                // duplicate the record to the end of overflow area
                // and replace the current one with added record
                impl::DataRecord newRecord(key, currentOverflowEndIx, value);

                appendToOverflowArea(
                        checkedRecord.getKey(),
                        checkedRecord.getData(),
                        checkedRecord.getPtr());

                data.insert(dataIx, newRecord);

                break;
            }

            // reached end of chain
            if(checkedRecord.getPtr() == constants::INCORRECT_RECORD_KEY){
                checkedRecord.setPtr(currentOverflowEndIx);
                data.insert(dataIx, checkedRecord);
                appendToOverflowArea(key, value);
                break;
            }

            // get next from chain
            dataIx = checkedRecord.getPtr();
        }
    }

    void IndexedFile::appendToOverflowArea(generic::key_t key, const std::string& value, generic::pointer_t ptr) {
        //assert( overflow area is not full )
        data.insert(currentOverflowEndIx, impl::DataRecord(key, ptr, value));
        currentOverflowEndIx++;
    }

    void IndexedFile::postInsertJob() {

    }
}
