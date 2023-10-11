#include "Record.h"

std::vector<uint8_t> sbd::impl::DataRecord::serialize() const {
    auto bytes = Record::serialize();
    std::transform(data.begin(), data.end(), std::back_inserter(bytes), [](const char& c) {
        return static_cast<uint8_t>(c);
    });
    return bytes;
}

sbd::impl::DataRecord sbd::impl::DataRecord::deserialize(std::vector<uint8_t> &bytes, uint64_t offset) {
    DataRecord deserialized;
    auto begin = bytes.begin() + offset;
    deserialized.key = (*begin) + (*(begin + 1) << 8) + (*(begin + 2) << 16) + (*(begin + 3) << 24);
    deserialized.ptr = *(begin + 4) + (*(begin + 5) << 8) + (*(begin + 6) << 16) + (*(begin + 7) << 24);
    deserialized.data = std::string(
            begin + constants::HEADER_SIZE,
            begin + constants::HEADER_SIZE + constants::DATA_SIZE);
    return deserialized;
}

std::vector<uint8_t> sbd::impl::Record::serialize() const {
    std::vector<uint8_t> bytes;
    bytes.reserve(constants::HEADER_SIZE);
    bytes.push_back(key & 0xFF);
    bytes.push_back((key >> 8) & 0xFF);
    bytes.push_back((key >> 16) & 0xFF);
    bytes.push_back((key >> 24) & 0xFF);
    bytes.push_back(ptr & 0xFF);
    bytes.push_back((ptr >> 8) & 0xFF);
    bytes.push_back((ptr >> 16) & 0xFF);
    bytes.push_back((ptr >> 24) & 0xFF);
    return bytes;
}
