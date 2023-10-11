#include <gtest/gtest.h>

#include <impl/Record.h>

using namespace sbd::impl;

class SerializationTest : public testing::Test {};

TEST_F(SerializationTest, testDataRecordSerialization){
    DataRecord dataRecord;
    dataRecord.setKey(0xB16B00B5);
    dataRecord.setPtr(0xDEFEC8);
    dataRecord.setData("GSL2137");
    auto bytes = dataRecord.serialize();
    auto deserializedRecord = DataRecord::deserialize(bytes);
    EXPECT_EQ(deserializedRecord.getKey(), dataRecord.getKey());
    EXPECT_EQ(deserializedRecord.getPtr(), dataRecord.getPtr());
    EXPECT_EQ(deserializedRecord.getData(), dataRecord.getData());
}

TEST_F(SerializationTest, testIndexRecordSerialization){
    IndexRecord indexRecord;
    indexRecord.setKey(0xB00B1E5);
    indexRecord.setPtr(0xB105F00D);
    auto bytes = indexRecord.serialize();
    auto deserializedRecord = DataRecord::deserialize(bytes);
    EXPECT_EQ(deserializedRecord.getKey(), indexRecord.getKey());
    EXPECT_EQ(deserializedRecord.getPtr(), indexRecord.getPtr());
}