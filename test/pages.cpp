#include <gtest/gtest.h>

#include <impl/IndexedFile.h>

using namespace sbd;

class PagesTest : public testing::Test {

};

TEST_F(PagesTest, testCreateFile){

}

TEST_F(PagesTest, testInsertBasicScenario){
    impl::IndexedFile indexedFile;
    indexedFile.insert(1, "GSL2137");
    indexedFile.insert(5, "GSL2138");
    indexedFile.insert(8, "GSL2139");
}

TEST_F(PagesTest, testInsertOverflowInside){
    impl::IndexedFile indexedFile;
    indexedFile.insert(1, "GSL2137");
    indexedFile.insert(5, "GSL2138");
    indexedFile.insert(8, "GSL2139");
    indexedFile.insert(6, "GSL2140");
}

TEST_F(PagesTest, testOverflowThroughFirstPage){
    impl::IndexedFile indexedFile;
    indexedFile.insert(1, "GSL2137");
    indexedFile.insert(5, "GSL2138");
    indexedFile.insert(8, "GSL2139");
    indexedFile.insert(10, "GSL2140");
    indexedFile.insert(12, "GSL2141");
}