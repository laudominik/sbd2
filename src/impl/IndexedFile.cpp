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
        size_t left = 0;
        size_t right = primaryPages - 1;
        size_t posInIndex{}, posInData{};
        generic::key_t foundKey{constants::INCORRECT_RECORD_KEY};
        while(left <= right){
            auto mid = (left + right)/2;
            auto currentRecord = index.get(mid);

            if(currentRecord.getKey() <= key){
                posInIndex = mid;
                posInData = currentRecord.getPtr() * constants::DATA_RECORD_PER_PAGE;
                left = mid + 1;
                foundKey = currentRecord.getKey();
            } else {
                if(right == 0) break;
                right = mid - 1;
            }
        }

//        while(posInIndex > 0 && foundKey != constants::INCORRECT_RECORD_KEY && index.get(posInIndex-1).getKey() == foundKey){
//            posInIndex--;
//        }
        return {posInIndex, posInData};
    }

    // add (key, value) to chain of record with index = dataIx
    void IndexedFile::addToChain(generic::key_t key, std::string value, size_t indexIx, size_t dataIx) {
        while(true){
            auto checkedRecord = data.get(dataIx);

            if(checkedRecord.getKey() == key){
                // LOG this case

                // no records were added so the counter shouldn't increment
                overflowRecords--;
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
        const size_t snew = ceil(static_cast<double>(primaryRecords + overflowRecords - deletedRecords) / static_cast<double>(maxRecordsPerPage));

        {
            IndexedFile tempFile(constants::TEMP_INDEX_FILE_NAME, constants::TEMP_DATA_FILE_NAME, snew);

            size_t recordsOnCurrentPage = 0u;
            size_t currentPage = 0u;
            size_t processedRecords = 0u;
            size_t currentRecordIx = 0u;
            size_t currentPrimaryRecordIx = 0u;

            while (processedRecords != primaryRecords + overflowRecords - deletedRecords) {
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

    std::optional<size_t> IndexedFile::find0(generic::key_t key) {
        auto [posInIndex, posInData] = getPositionFromIndex(key);

        size_t posOnPage;
        bool smallest = true;
        for(auto i = 0u; i < constants::DATA_RECORD_PER_PAGE; i++){
            auto checkedRecord = data.get(i + posInData);
            if(checkedRecord.getKey() < key){
                smallest = false;
                posOnPage = i;
            }
            if(checkedRecord.getKey() == key){
                // LOG this case
                return i + posInData;
            }
        }
        if(smallest){
            return std::nullopt;
        }

        auto currentIx = posInData + posOnPage;
        while(true){
            auto currentRecord = data.get(currentIx);
            if(currentRecord.getKey() == key){
                return currentIx;
            }
            if(currentRecord.getPtr() == constants::INCORRECT_RECORD_KEY){
                return std::nullopt;
            }
            currentIx = currentRecord.getPtr();
        }
    }

    std::string IndexedFile::find(generic::key_t key) {
        auto ix = find0(key);
        if(!ix){
            return "";
        }

        return data.get(*ix).getData();
    }

    void IndexedFile::remove(generic::key_t key) {
        const auto [posInIndex, posInData] = getPositionFromIndex(key);

        size_t posOnPage = 0;
        bool smallest = true;

        for(auto i = 0u; i < constants::DATA_RECORD_PER_PAGE; i++){
            auto checkedRecord = data.get(i + posInData);
            if(checkedRecord.getKey() <= key){
                smallest = false;
                posOnPage = i;
            }
        }

        if(smallest){
            // key not present
            return;
        }

        auto currentPos = posOnPage + posInData;

        auto currentRecord = data.get(currentPos);
        if(currentRecord.getKey() == key){
            if(currentRecord.getPtr() == constants::INCORRECT_RECORD_KEY){
                // shift entire page to the left
                for(auto i = posOnPage; i < constants::DATA_RECORD_PER_PAGE-1; i++){
                    data.insert(i+posInData, data.get(i+posInData+1));
                }

                // corner case, we should handle empty pages
//                bool pageEmpty = true;
//                for(auto i = 0u; i < constants::DATA_RECORD_PER_PAGE; i++){
//                    if(data.get(i).getKey() != constants::INCORRECT_RECORD_KEY){
//                        pageEmpty = false;
//                    }
//                }
//
//                if(pageEmpty){
//
//                    // TODO: index should have a value set to
//                }
                data.insert(posInData + constants::DATA_RECORD_PER_PAGE - 1, {constants::INCORRECT_RECORD_KEY, constants::INCORRECT_RECORD_KEY, ""});

                // update index
                index.insert(posInIndex, {data.get(posInData).getKey(), index.get(posInIndex).getPtr()});
                deletedRecords++;

                // TODO: handle case of page getting empty
                // explanation: if the page gets empty then it will probably cause searching index to fail, ignoring it for now

                return;
            } else {
                auto nextRecord = data.get(currentRecord.getPtr());
                /*
                 * it leaks memory under address of getPtr() but we're ok with that, it's gonna get freed on reorganisation
                 */
                data.insert(currentPos, nextRecord);
                index.insert(posInIndex, {data.get(posInData).getKey(), index.get(posInIndex).getPtr()});
                deletedRecords++;
                return;
            }
        }

        if(currentRecord.getPtr() == constants::INCORRECT_RECORD_KEY){
            // key not present
            return;
        }


        // key may be somewhere in overflow
        auto previousPos = currentPos;
        currentPos = currentRecord.getPtr();

        while (true){
            currentRecord = data.get(currentPos);
            if(currentRecord.getKey() == key){
                /*
                 * it leaks memory under currentPos, but it will get freed
                 */
                auto previousRecord = data.get(previousPos);
                previousRecord.setPtr(currentRecord.getPtr());
                data.insert(previousPos, previousRecord);
                deletedRecords++;
                return;
            }

            previousPos = currentPos;
            currentPos = currentRecord.getPtr();
            if(currentPos == constants::INCORRECT_RECORD_KEY){
                // key not present
                return;
            }
        }
    }

    // TODO: test it
    void IndexedFile::update(generic::key_t key, generic::key_t newKey, const std::string &value) {
        if(key == newKey){
            // just update
            auto ix = find0(key);
            if(!ix){
                // LOG that
                return;
            }
            // setup
            return;
        }
        remove(key);
        insert(newKey, value);
    }

}
