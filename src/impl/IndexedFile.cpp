#include "IndexedFile.h"

#include <cmath>
#include <utility>

#include <util/Constants.h>

namespace sbd::impl {
    IndexedFile::IndexedFile(): IndexedFile(
            constants::INDEX_FILE,
            constants::DATA_FILE_NAME,
            constants::INITIAL_PAGES_COUNT) {}

    IndexedFile::IndexedFile(const std::string& indexFileName_, const std::string& dataFileName_, size_t numberOfPrimaryPages):
        primaryPages(numberOfPrimaryPages),
        overflowPages(static_cast<size_t>(ceil(numberOfPrimaryPages * constants::OVERFLOW_RATIO))),
        indexFileName(indexFileName_),
        dataFileName(dataFileName_),
        index(indexFileName_),
        data(dataFileName_){

        clear();
        // evenly distribute the key values on pages
        const auto DISTANCE = constants::MAX_RECORD_KEY / numberOfPrimaryPages;
        for(auto pageNumber = 0u; pageNumber < numberOfPrimaryPages; pageNumber++){
            index.push_back(IndexRecord(pageNumber * DISTANCE, pageNumber));
            data.appendEmptyPage();
        }

        for(auto pageNumber = 0u; pageNumber < overflowPages; pageNumber++){
            data.appendEmptyPage();
        }
        currentOverflowEndIx = numberOfPrimaryPages * constants::DATA_RECORD_PER_PAGE;

        index.flushCachedPage();
        data.flushCachedPage();
    }

    void IndexedFile::clear() {
        std::fstream f1;
        f1.open(indexFileName, std::ios::out);
        f1<<"";
        f1.close();
        std::fstream f2;
        f2.open(dataFileName, std::ios::out);
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
        if(data.get(posInData).getKey() == constants::INCORRECT_RECORD_KEY){
            IndexRecord newIndexRecord{indexRecord};
            newIndexRecord.setKey(key);
            index.insert(posInIndex, newIndexRecord);

            DataRecord newDataRecord{key, constants::INCORRECT_RECORD_KEY, value};
            data.insert(posInData, newDataRecord);
            primaryRecords++;
            postInsertJob();
            return;
        }

        // find suitable position for record
        auto posOnPage = 0u;
        bool smallest = true;
        for(auto i = 0u; i < constants::DATA_RECORD_PER_PAGE; i++){
            auto checkedRecord = data.get(i + posInData);
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
            overflowRecords++;
            return;
        }

        posOnPage++;
        if(posOnPage < constants::DATA_RECORD_PER_PAGE ){
            // put it in empty place
            if(data.get(posOnPage + posInData).getKey() == constants::INCORRECT_RECORD_KEY){
                data.insert(posOnPage + posInData, impl::DataRecord(key, constants::INCORRECT_RECORD_KEY, value));
                primaryRecords++;
            } else {
                // add to chain of posOnPage - 1
                addToChain(key, value, posInIndex, posOnPage - 1 + posInData);
                overflowRecords++;
            }
        } else {
            // add to chain of last element
            addToChain(key, value, posInIndex, posOnPage - 1 + posInData);
            overflowRecords++;
        }

        postInsertJob();
    }

    // gets index of first record of suitable page in DATA FILE and index of the corresponding record in INDEX FILE
    std::pair<size_t, size_t> IndexedFile::getPositionFromIndex(generic::key_t key) {
        // TODO: change to bisection
        size_t posInIndex{}, posInData{};
        for(auto i = 0u; i < primaryPages; i++){
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
        // check if overflow area is full
        if(currentOverflowEndIx < (primaryPages +  overflowPages) * constants::DATA_RECORD_PER_PAGE){
            return;
        }
        reorganise();
    }

    void IndexedFile::reorganise(double alpha) {
        const size_t maxRecordsPerPage = floor(constants::DATA_RECORD_PER_PAGE * alpha);
        const size_t snew = ceil(static_cast<double>(primaryRecords + overflowRecords) / static_cast<double>(maxRecordsPerPage));

        {
            IndexedFile tempFile(constants::TEMP_INDEX_FILE_NAME, constants::TEMP_DATA_FILE_NAME, snew);

            const size_t allRecordsCount = (primaryPages + overflowPages) * constants::DATA_RECORD_PER_PAGE;

            size_t recordsOnCurrentPage = 0u;
            size_t currentPage = 0u;
            size_t processedRecords = 0u;
            size_t currentRecordIx = 0u;
            size_t currentPrimaryRecordIx = 0u;

            while (processedRecords != primaryRecords + overflowRecords) {
                auto currentRecord = data.get(currentRecordIx);
                auto key = currentRecord.getKey();
                auto ptr = currentRecord.getPtr();
                auto value = currentRecord.getData();

                if (currentRecord.getKey() == constants::INCORRECT_RECORD_KEY) {
                    currentPrimaryRecordIx++;
                    currentRecordIx = currentPrimaryRecordIx;
                    continue;
                }

                // handle next page
                if (recordsOnCurrentPage >= maxRecordsPerPage) {
                    currentPage++;
                    recordsOnCurrentPage = 0;
                }
                if (recordsOnCurrentPage == 0u) {
                    tempFile.index.insert(currentPage, impl::IndexRecord(currentRecord.getKey(), currentPage));
                }

                // new record insertion
                tempFile.data.insert(currentPage * constants::DATA_RECORD_PER_PAGE + recordsOnCurrentPage,
                                     impl::DataRecord(key, constants::INCORRECT_RECORD_KEY, value));
                recordsOnCurrentPage++;
                processedRecords++;

                // which record to process next
                if (ptr == constants::INCORRECT_RECORD_KEY) {
                    currentPrimaryRecordIx++;
                    currentRecordIx = currentPrimaryRecordIx;
                } else {
                    currentRecordIx = ptr;
                }
            }

            primaryPages = tempFile.primaryPages;
            overflowPages = tempFile.overflowPages;
            currentOverflowEndIx = tempFile.currentOverflowEndIx;
            primaryRecords += overflowRecords;
            overflowRecords = 0;
        }

        std::remove(constants::INDEX_FILE);
        std::remove(constants::DATA_FILE_NAME);
        std::rename(constants::TEMP_INDEX_FILE_NAME, constants::INDEX_FILE);
        std::rename(constants::TEMP_DATA_FILE_NAME, constants::DATA_FILE_NAME);
        index.reset();
        data.reset();
    }

    void IndexedFile::reorganise() {
        reorganise(constants::REORGANISATION_ALPHA);
    }

    void IndexedFile::find0(generic::key_t key) {

    }
}
