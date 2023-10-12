#include "Index.h"

#include <fstream>
#include <vector>

#include <util/Constants.h>
#include <time/DiskClocks.h>

namespace {
    void writePageToFile(std::vector<uint8_t>& page, std::fstream& file) {
        // padding
        while(page.size() != sbd::constants::PAGE_SIZE){
            page.push_back(0x0u);
        }
        sbd::time::writeClock().tick();
        std::copy(page.begin(), page.end(), std::ostream_iterator<uint8_t>(file));
    }
}

namespace sbd::impl {

    void Index::flush() const {
        std::fstream indexStream;
        indexStream.open(fileName, std::ios::out | std::ios::binary);
        if(!indexStream.good()) {
            throw std::runtime_error("[ERROR] cannot open file");
        }

        std::vector<uint8_t> pageSerialized;
        pageSerialized.reserve(constants::PAGE_SIZE);
        static constexpr auto RECORDS_PER_PAGE = constants::PAGE_SIZE/constants::HEADER_SIZE;
        auto recordsInPage = 0u;

        for(const auto& record : data) {
            auto serial = record.serialize();
            pageSerialized.insert(pageSerialized.end(), serial.begin(), serial.end());
            recordsInPage++;
            if(recordsInPage == RECORDS_PER_PAGE) {
                writePageToFile(pageSerialized, indexStream);
                recordsInPage = 0u;
                pageSerialized.clear();
            }
        }

        if(!pageSerialized.empty()) {
            writePageToFile(pageSerialized, indexStream);
        }

        indexStream.close();
    }




}