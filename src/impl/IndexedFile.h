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
        void update(const Record& record);
        Record find(generic::key_t);
        void add(const Record& record);

    private:
        void allocateDiskSpace();
//        Index index;
//        Data data;
        generic::File<IndexRecord> index;
//        generic::File<DataRecord> data;
    };
}


