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

        std::pair<size_t, size_t> getPositionFromIndex(generic::key_t key);

        generic::File<IndexRecord> index;
        generic::File<DataRecord> data;
    };
}


