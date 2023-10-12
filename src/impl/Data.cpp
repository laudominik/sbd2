#pragma once

#include "Data.h"

#include <fstream>

#include <util/Constants.h>
#include <time/DiskClocks.h>

sbd::impl::Data::Data(std::string filename, size_t numPages): filename(std::move(filename)) {
    for(auto i = 0u; i < numPages; i++){ addEmptyPage(); }
}

void sbd::impl::Data::addEmptyPage() {
    std::fstream dataStream;
    dataStream.open(filename, std::ios::app | std::ios::binary);
    if(!dataStream.good()) {
        throw std::runtime_error("[ERROR] cannot open file");
    }

    std::vector<uint8_t> page(constants::PAGE_SIZE, 0xFF);
    sbd::time::writeClock().tick();
    std::copy(page.begin(), page.end(), std::ostream_iterator<uint8_t>(dataStream));

    dataStream.close();
}

