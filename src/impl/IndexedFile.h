#pragma once


#include <impl/Record.h>
#include <util/Constants.h>
#include <generic/File.h>


namespace sbd::impl {
    class IndexedFile {
    public:
        IndexedFile();

        /*
         * basic operations
         */
        Record find(generic::key_t key);
        void remove(generic::key_t key);
        void update(const Record& record);
        void insert(generic::key_t key, const std::string& value);
        void reorganise();
        void clear();

    private:
        void allocateDiskSpace();

        void addToChain(generic::key_t key, const std::string value, size_t indexIx, size_t dataIx);

        std::pair<size_t, size_t> getPositionFromIndex(generic::key_t key);
        // check if file is full or overflow area is full ->
        void postInsertJob();
        void appendToOverflowArea(generic::key_t key, const std::string& value, generic::pointer_t ptr = constants::INCORRECT_RECORD_KEY);

        generic::File<IndexRecord> index;
        generic::File<DataRecord> data;
        size_t currentOverflowEndIx{0};
        size_t startOfOverflowArea{0};
    };
}


