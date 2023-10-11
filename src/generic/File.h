#pragma once

#include <string>
#include <optional>
#include <utility>
#include <vector>

namespace sbd::generic {
    template<typename RECORD>
    class File {
    public:
        explicit File(std::string  filename): filename(std::move(filename)) {
            /*
             * TODO:
             * - allocate disk space for primary records
             * - allocate disk space for overflow area
             */
        }

        /*
         * adding new block
         */
        void reorganise(){

        }

        /*
         * TODO:
         *  - add record to page
         *  - or add record to overflow area
         */
        void insert(generic::pointer_t pageNumber, impl::Record record){
            // loadPage(pageNumber)
            // if page.full || cant add at the end
            //   add record to overflow area, remember page number
            //   find direct predecessor of record, remember it's first chain ptr
            //   go to the last chained record of the direct predecessor, update
            //   to the remembered page number
            // else -> add at page's end
        }

        /*
         * TODO:
         * - get record with key from the page
         * - also search through overflow area
         */
        std::optional<RECORD> get(generic::pointer_t pageNumber, generic::key_t key){
            // check if the key is in page (simple iteration) -> if so return
            // check if the key is in overflow, use get(
        }

        /*
         * TODO:
         * - set record's value if it's present in the page
         * - if not, recursive call update on the page from likely pointer
         */
        void update(generic::pointer_t pageNumber, generic::key_t key, const std::string& value){

        }

        /*
         * TODO:
         *  - find record with key to delete
         */
        bool remove(generic::pointer_t pageNumber, generic::key_t key){

        }

        /*
         * TODO:
         * load page with provided index to cache
         * if the page is already present in cache, flush it
         * if not, flush the current page and then load the one with index
         * should be protected
         */
        void loadPage(generic::pointer_t index){
            // flush page
            // if (index == cachedPageIndex) return;
            // open file
            // fseek to position index * block size
            // load block and deserialize it
            // close file
            // cachedPageIndex = index
        }

        /*
         * TODO:
         * flush page to memory, keeping in mind it's index
         * should be protected
         */
        void flushPage(){
            // if(dirty)
            // open file
            // fseek to cachedPageIndex * block size
            // serialize current page
            // write to file
            // close file
            // dirty = false
        }

    protected:
        const std::string filename;
        uint32_t primaryRecords;
        uint32_t overflowRecords;

        std::vector<uint8_t> cachedPage;
        generic::pointer_t cachedPageIndex;
        bool dirty{false};

    };
}

