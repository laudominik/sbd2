#pragma once

#include <vector>

#include <impl/Record.h>

namespace sbd::impl {

        class Index {
        public:
            explicit Index(std::string fileName): fileName(std::move(fileName)){}
            void flush() const;
            void addRecord(const IndexRecord& record){
                data.push_back(record);
            }
        private:
            std::vector<IndexRecord> data;
            std::string fileName;
        };
}
