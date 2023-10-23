#include "IndexedFile.h"

#include <cmath>
#include <utility>
#include <iostream>

#include <util/Constants.h>

void logKeyAlreadyExists(uint32_t key){
    std::cout << "[WARNING] record with key "<< key << " already exists, aborting..." << std::endl;
}

void logNoSuchKey(uint32_t key){
    std::cout << "[WARNING] record with key "<< key << " not present in file, aborting..." << std::endl;
}


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

    bool IndexedFile::insert(generic::key_t key, const std::string& value) {
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
            return true;
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
                logKeyAlreadyExists(key);
                return false;
            }
        }

        // explicit handling of corner case, where the smallest record will go behind the first record of the first page
        if(smallest){
            addToChain(key, value, posInData, posInIndex);
            indexRecord.setKey(key);
            index.insert(posInIndex, indexRecord);
            postInsertJob();
            overflowRecords++;
            return true;
        }

        posOnPage++;
        if(posOnPage < constants::DATA_RECORD_PER_PAGE ){
            // put it in empty place
            if(data.get(posOnPage + posInData).getKey() == constants::INCORRECT_RECORD_KEY){
                // if there's a bigger element in the overflow chain, it should be added to the overflow

                auto currentPos = data.get(posOnPage + posInData - 1).getPtr();
                while(currentPos != constants::INCORRECT_RECORD_KEY){
                    auto currentRecord = data.get(currentPos);
                    if(currentRecord.getKey() < key){
                        currentPos = currentRecord.getPtr();
                        continue;
                    }
                    if(currentRecord.getKey() == key){
                        logKeyAlreadyExists(key);
                        return false;
                    }

                    impl::DataRecord newRecord(key, currentOverflowEndIx, value);

                    appendToOverflowArea(
                            currentRecord.getKey(),
                            currentRecord.getData(),
                            currentRecord.getPtr());

                    data.insert(currentPos, newRecord);
                    overflowRecords++;
                    postInsertJob();
                    return true;
                }

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
        return true;
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
                if(mid == 0) break;
                right = mid - 1;
            }
        }

        return {posInIndex, posInData};
    }

    // add (key, value) to chain of record with index = dataIx
    void IndexedFile::addToChain(generic::key_t key, std::string value, size_t indexIx, size_t dataIx) {
        while(true){
            auto checkedRecord = data.get(dataIx);

            if(checkedRecord.getKey() == key){
                logKeyAlreadyExists(key);

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
        size_t snew = ceil(static_cast<double>(primaryRecords + overflowRecords - deletedRecords) / static_cast<double>(maxRecordsPerPage));
        if(snew == 0){
            return;
        }

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

    bool IndexedFile::remove(generic::key_t key) {
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
            logNoSuchKey(key);
            return false;
        }

        auto currentPos = posOnPage + posInData;

        auto currentRecord = data.get(currentPos);
        if(currentRecord.getKey() == key){
            if(currentRecord.getPtr() == constants::INCORRECT_RECORD_KEY){
                // shift entire page to the left
                for(auto i = posOnPage; i < constants::DATA_RECORD_PER_PAGE-1; i++){
                    data.insert(i+posInData, data.get(i+posInData+1));
                }
                data.insert(posInData + constants::DATA_RECORD_PER_PAGE - 1, {constants::INCORRECT_RECORD_KEY, constants::INCORRECT_RECORD_KEY, ""});

                // update index
                index.insert(posInIndex, {data.get(posInData).getKey(), index.get(posInIndex).getPtr()});
                deletedRecords++;
                return true;
            } else {
                auto nextRecord = data.get(currentRecord.getPtr());
                /*
                 * it leaks memory under address of getPtr() but we're ok with that, it's gonna get freed on reorganisation
                 */
                data.insert(currentPos, nextRecord);
                index.insert(posInIndex, {data.get(posInData).getKey(), index.get(posInIndex).getPtr()});
                deletedRecords++;
                return true;
            }
        }

        if(currentRecord.getPtr() == constants::INCORRECT_RECORD_KEY){
            // key not present
            return false;
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
                return true;
            }

            previousPos = currentPos;
            currentPos = currentRecord.getPtr();
            if(currentPos == constants::INCORRECT_RECORD_KEY){
                logNoSuchKey(key);
                return false;
            }
        }
    }

    void IndexedFile::update(generic::key_t key, generic::key_t newKey, const std::string &value) {
        if(key == newKey){
            // just update
            auto ix = find0(key);
            if(!ix){
                logNoSuchKey(key);
                return;
            }
            // setup
            auto record = data.get(*ix);
            record.setData(value);
            data.insert(*ix, record);
            return;
        }
        remove(key) && insert(newKey, value);
    }

    std::ostream &operator<<(std::ostream &os, IndexedFile& indexedFile) {
        indexedFile.blockPrint(os);
        return os;
    }

    void IndexedFile::inorderPrint(std::ostream& os) {
        time::readClock().freeze();
        time::writeClock().freeze();
        os << "___INDEXED__FILE___" << std::endl;

        size_t currentRecordIx = 0u;
        size_t currentPrimaryRecordIx = 0u;
        size_t processedRecords = 0u;
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
            os << "#" << currentRecordIx << " key: " << currentRecord.getKey() << " data: " << currentRecord.getData() << std::endl;
            //os << " key: " << currentRecord.getKey() << " data: " << currentRecord.getData() << std::endl;
            processedRecords++;

            // which record to process next
            if (ptr == constants::INCORRECT_RECORD_KEY) {
                currentPrimaryRecordIx++;
                currentRecordIx = currentPrimaryRecordIx;
            } else {
                currentRecordIx = ptr;
            }
        }

        time::readClock().unfreeze();
        time::writeClock().unfreeze();
    }

    void IndexedFile::blockPrint(std::ostream &os) {
        time::readClock().freeze();
        time::writeClock().freeze();

        data.flushCachedPage();
        index.flushCachedPage();
        generic::File<IndexRecord> index(indexFileName);
        generic::File<DataRecord> data(dataFileName);
        os << "___INDEXED__FILE___" << std::endl;
        os << "-------INDEX-------" << std::endl;
        auto indexPageCounter = 0;
        for(auto i = 0u; i < primaryPages; i++){
            if(i % (constants::PAGE_SIZE / constants::HEADER_SIZE) == 0){
                os << "=======PAGE" << indexPageCounter <<"======"<< std::endl;
                indexPageCounter++;
            }
            auto currentRecord = index.get(i);
            os << "page: " << currentRecord.getPtr() << " key: " << currentRecord.getKey() << std::endl;
        }
        os << "-----INDEX-END-----" << std::endl << std::endl;
        os << "------PRIMARY------" << std::endl;
        auto primaryPageCounter = 0;
        auto allPrimaryRecords = primaryPages * constants::DATA_RECORD_PER_PAGE;
        for(auto i = 0u; i < allPrimaryRecords; i++){
            if(i % constants::DATA_RECORD_PER_PAGE == 0){
                os << "=======PAGE" << primaryPageCounter <<"======"<< std::endl;
                primaryPageCounter++;
            }
            auto currentRecord = data.get(i);
            if(currentRecord.getKey() == constants::INCORRECT_RECORD_KEY){
                os <<"#" << i << " *******************" << std::endl;
                continue;
            }
            os << "#" << i << " key: " << currentRecord.getKey() << " data: " << currentRecord.getData();
            if (currentRecord.getPtr() != constants::INCORRECT_RECORD_KEY){
                os << " ptr: " << currentRecord.getPtr();
            }
            os << std::endl;
        }
        os << "----PRIMARY-END----" << std::endl << std::endl;
        os << "-----OVERFLOW------" << std::endl;
        auto overflowPageCounter = 0;
        auto allOverflowRecords = overflowPages * constants::DATA_RECORD_PER_PAGE;
        for(auto i = allPrimaryRecords; i < allPrimaryRecords + allOverflowRecords; i++){
            if(i % constants::DATA_RECORD_PER_PAGE == 0){
                os << "=======PAGE" << overflowPageCounter <<"======"<< std::endl;
                overflowPageCounter++;
            }
            auto currentRecord = data.get(i);
            if(currentRecord.getKey() == constants::INCORRECT_RECORD_KEY){
                os <<"#" << i << " *******************" << std::endl;
                continue;
            }
            os << "#" << i << " key: " << currentRecord.getKey() << " data: " << currentRecord.getData();
            if (currentRecord.getPtr() != constants::INCORRECT_RECORD_KEY){
                os << " ptr: " << currentRecord.getPtr();
            }
            os << std::endl;
        }
        os << "----OVERFLOW-END---" << std::endl;

        time::readClock().unfreeze();
        time::writeClock().unfreeze();
    }

    void IndexedFile::reportSize(std::ostream &os) const {
        os << "$$$$$$$REPORT$$$$$$$$" << std::endl;
        size_t ixPerPage = constants::PAGE_SIZE/constants::HEADER_SIZE;
        size_t numOfPages = ceil(static_cast<double>(primaryPages) / static_cast<double>(ixPerPage));

        os << "index.size       =" <<  numOfPages * constants::PAGE_SIZE <<"B" << std::endl;
        os << "primary.size     =" << primaryPages * constants::PAGE_SIZE <<"B"<< std::endl;
        os << "overflow.size    =" << overflowPages * constants::PAGE_SIZE << "B" << std::endl;
    }

}
