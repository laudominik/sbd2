#pragma once


#include <impl/Record.h>
#include <util/Constants.h>
#include <generic/File.h>


namespace sbd::impl {
    class IndexedFile {
    public:
        IndexedFile();
        explicit IndexedFile(const std::string& indexFileName, const std::string& dataFileName, size_t numberOfPrimaryPages);

        /*
         * basic operations
         */
        std::string find(generic::key_t key);
        bool remove(generic::key_t key);
        void update(generic::key_t key, generic::key_t newKey, const std::string& value);
        bool insert(generic::key_t key, const std::string& value);
        void reorganise();
        void reorganise(double alpha);
        void clear();

        friend std::ostream& operator<<(std::ostream& os, IndexedFile& indexedFile);
    private:
        void allocateDiskSpace();
        std::optional<size_t> find0(generic::key_t key);

        void addToChain(generic::key_t key, const std::string value, size_t indexIx, size_t dataIx);

        std::pair<size_t, size_t> getPositionFromIndex(generic::key_t key);
        // check if file is full or overflow area is full ->
        void postInsertJob();
        void appendToOverflowArea(generic::key_t key, const std::string& value, generic::pointer_t ptr = constants::INCORRECT_RECORD_KEY);

        std::string indexFileName, dataFileName;
        generic::File<IndexRecord> index;
        generic::File<DataRecord> data;
        size_t currentOverflowEndIx, primaryPages, overflowPages, primaryRecords{}, overflowRecords{}, deletedRecords{};
    };
}


