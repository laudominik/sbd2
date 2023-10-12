#pragma once

#include <string>
#include <optional>
#include <utility>
#include <vector>
#include <fstream>

#include <util/Constants.h>
#include <time/DiskClocks.h>

namespace sbd::generic {


    /*
     * file supporting paging
     */
    template<typename RECORD_T>
    class File {
    public:
        explicit File(std::string filename): filename(std::move(filename)){}

        RECORD_T get(size_t ix){
            auto [pageNum, ixOnPage] = getPageForRecordIndex(ix);
            loadPageToCache(pageNum);
            return cachedPage.data[ixOnPage];
        }

        RECORD_T set(size_t ix, const RECORD_T& record){
            auto [pageNum, ixOnPage] = getPageForRecordIndex(ix);
            loadPageToCache(pageNum);
            cachedPage.data[ixOnPage] = record;
            cachedPage.dirty = true;
        }

        struct page_t {
            std::vector<RECORD_T> data;
            size_t index{};
            mutable bool dirty{true};
            bool isPresent{false};
        } cachedPage;

        void flushCachedPage() const {
            if(!cachedPage.dirty || !cachedPage.isPresent) return;
            std::fstream file;

            file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
            if(!file.good()){
                throw std::runtime_error("[ERROR] problem in opening file");
            }

            std::vector<uint8_t> pageSerialized;
            pageSerialized.reserve(constants::PAGE_SIZE);
            for(const auto& record : cachedPage.data){
                auto serial = record.serialize();
                pageSerialized.insert(pageSerialized.end(), serial.begin(), serial.end());
            }
            while(pageSerialized.size() != constants::PAGE_SIZE){
                pageSerialized.push_back(0xFF);
            }
            fseek(file, cachedPage.index * constants::PAGE_SIZE);
            std::copy(pageSerialized.begin(), pageSerialized.end(), std::ostream_iterator<uint8_t>(file));
            file.close();
            cachedPage.dirty = false;
            time::writeClock().tick();
        }

    private:
        std::string filename;

        std::pair<size_t, size_t> getPageForRecordIndex(size_t ix) const {
            const auto recordsPerPage = constants::PAGE_SIZE / RECORD_T::size() ;
            return std::make_pair(static_cast<size_t>(ix / recordsPerPage), ix % recordsPerPage);
        }

        void loadPageToCache(size_t pageNum){
             if(pageNum == cachedPage.index && cachedPage.isPresent) return;
             flushCachedPage();
             cachedPage.data.clear();
             cachedPage.dirty = false;
             cachedPage.isPresent = true;
             cachedPage.index = pageNum;
             std::fstream file;
             file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
             if(!file.good()){
                 throw std::runtime_error("[ERROR] problem in opening file");
             }
             std::vector<uint8_t> pageSerialized(constants::PAGE_SIZE);
             fseek(file, pageNum * constants::PAGE_SIZE);
             file.read(reinterpret_cast<char*>(pageSerialized.data()), constants::PAGE_SIZE);
             auto readBytes = file.gcount();

             // also omits padding
             for(auto i = 0u; i <  constants::PAGE_SIZE / RECORD_T::size(); i++){
                cachedPage.data.push_back(RECORD_T::deserialize(pageSerialized, i * RECORD_T::size()));
             }
             time::readClock().tick();
         }

         // emulate fseek
         void fseek(std::fstream& file, auto offset) const {
             file.seekg(offset);
         }
    };
}

