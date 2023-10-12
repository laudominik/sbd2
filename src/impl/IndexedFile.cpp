//
// Created by Dominik Lau on 11/10/2023.
//

#include "IndexedFile.h"

#include <util/Constants.h>

sbd::impl::IndexedFile::IndexedFile() : index(constants::INDEX_FILE), data(constants::DATA_FILE_NAME, constants::INITIAL_PAGES_COUNT){
    auto pageNumber = 0;
    for(auto i = 0u; i < pageNumber; i++){
        index.addRecord({constants::INCORRECT_RECORD_KEY, constants::INCORRECT_RECORD_KEY});
    }
    index.flush();
}

void sbd::impl::IndexedFile::add(const sbd::impl::Record &record) {

}
