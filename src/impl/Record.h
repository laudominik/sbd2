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
        Record(generic::key_t key, generic::pointer_t ptr): key(key), ptr(ptr){}
        Record() = default;

        [[nodiscard]] virtual std::vector<uint8_t> serialize() const;

        static size_t size(){
            return constants::HEADER_SIZE;
        }

        static Record deserialize(std::vector<uint8_t>& bytes, uint64_t offset = 0);

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

        virtual bool isInvalid(){
            return key == constants::INCORRECT_RECORD_KEY && ptr == constants::INCORRECT_RECORD_KEY;
        }

    protected:
        generic::key_t key{};
        generic::pointer_t ptr{};

    };

    class DataRecord : public Record {

    public:
        DataRecord(generic::key_t key, generic::pointer_t ptr, const std::string& data) : Record(key, ptr), data(data) {};
        DataRecord() = default;
        [[nodiscard]] std::vector<uint8_t> serialize() const override;
        static DataRecord deserialize(std::vector<uint8_t>& bytes, uint64_t offset = 0);

        void setData(const std::string& data_){
            data = data_;
        }
        [[nodiscard]] std::string getData() const {
            return data;
        }

        static size_t size(){
            return constants::DATA_RECORD_SIZE;
        }
    protected:
        std::string data;
    };

    using IndexRecord = Record;
}
