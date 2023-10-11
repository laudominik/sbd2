#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <generic/Pointer.h>
#include <generic/Key.h>
#include <util/Constants.h>

namespace sbd::impl {
    class Record {
    public:
        [[nodiscard]] virtual std::vector<uint8_t> serialize() const;

        void setKey(generic::key_t key_){
            key = key_;
        }
        void setPtr(generic::pointer_t ptr_){
            ptr = ptr_;
        }
        [[nodiscard]] generic::key_t  getKey() const{
            return key;
        }

        [[nodiscard]] generic::pointer_t getPtr() const {
            return ptr;
        }

    protected:
        generic::key_t key;
        generic::pointer_t ptr;

    };

    class DataRecord : public Record {

    public:
        [[nodiscard]] std::vector<uint8_t> serialize() const override;
        static DataRecord deserialize(std::vector<uint8_t>& bytes, uint64_t offset = 0);

        void setData(const std::string& data_){
            data = data_;
        }
        [[nodiscard]] std::string getData() const {
            return data;
        }
    protected:
        std::string data;
    };

    using IndexRecord = Record;
}
