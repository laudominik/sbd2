#include <impl/Record.h>
#include <util/Constants.h>
#include <impl/Index.h>
#include <impl/Data.h>
#include <impl/IndexedFile.h>
#include <generic/File.h>

using namespace sbd;

int main(){
//    impl::IndexedFile indexedFile;
    generic::File<impl::IndexRecord> index(constants::INDEX_FILE);

    auto rec = index.get(3);
    auto rec2 = index.get(7);
    auto rec3 = index.get(2);
    auto rec4 = index.get(9);

    index.set(6, {0xB16B00B5, 0xB105F00D});
    index.flushCachedPage();
//    for(int i = 0; i < 7; i++){
//        index.cachedPage.data.emplace_back(0xCCCCCCCC, 0xDDDDDDDD);
//    }
//
//    auto y = index[3];
//    index.cachedPage.index = 1;
//    index[3];
//    index.cachedPage.index = 2;
//    index[3];
//
//    index.cachedPage.data.clear();
//    for(int i = 0; i < 7; i++){
//        index.cachedPage.data.emplace_back(0xBBBBBBBB, 0xAAAAAAAA);
//    }
//    index.cachedPage.index = 1;
//    index[3];

}