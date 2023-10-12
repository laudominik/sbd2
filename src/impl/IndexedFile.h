#pragma once


#include <impl/Record.h>
#include <impl/Index.h>
#include <impl/Data.h>
#include <util/Constants.h>


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
        Index index;
        Data data;

    };
}


