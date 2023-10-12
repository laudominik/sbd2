#pragma once

#include <cstddef>

#include <impl/Record.h>

namespace sbd::impl {
    class Data {
    public:
        explicit Data(std::string filename ,size_t numPages);

    private:
        std::string filename;
        std::vector<impl::DataRecord> currentPage;

        void addEmptyPage();
    };
}