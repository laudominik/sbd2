#include "IndexedFile.h"

#include <util/Constants.h>

namespace sbd::impl {
    IndexedFile::IndexedFile(): index(constants::INDEX_FILE), data(constants::DATA_FILE_NAME) {
        clear();
        for(auto pageNumber = 0u; pageNumber < constants::INITIAL_PAGES_COUNT; pageNumber++){
            index.push_back(IndexRecord(constants::INCORRECT_RECORD_KEY, pageNumber));
            data.appendEmptyPage();
        }
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
        // TODO:
        // 1. search through the index for correct position
        //      1.to handle: adding to empty index page (idea: incorrect key is the biggest)
        //          if the page is empty, update index with the key value of the element
        // 2. add to the position
        //  2.1 see if something is under that position
        //  2.2 find the correct free place for the new record inside the block
        //  2.3 if the correct place is occupied, add to the overflow chain
        auto [posInIndex, posInData] = getPositionFromIndex(key);
        auto indexRecord = index.get(posInIndex);
        if(indexRecord.getKey() == constants::INCORRECT_RECORD_KEY){
            IndexRecord newIndexRecord{indexRecord};
            newIndexRecord.setKey(key);
            index.insert(posInIndex, newIndexRecord);

            DataRecord newDataRecord{key, constants::INCORRECT_RECORD_KEY, value};
            data.insert(posInData, newDataRecord);
            return;
        }
    }

    std::pair<size_t, size_t> IndexedFile::getPositionFromIndex(generic::key_t key) {
        return {0, 0};
    }
}
