#include <iostream>

#include <time/DiskClocks.h>
#include <impl/IndexedFile.h>


using namespace sbd;

int main(){
    impl::IndexedFile indexedFile;
    indexedFile.insert(1, "GSL2137");
    indexedFile.insert(4, "GSL2138");
    indexedFile.insert(9, "GSL2139");
    indexedFile.insert(10, "GSL2222");
    indexedFile.insert(11, "GSL6969");

//    indexedFile.insert(6, "GSL6969");
//    indexedFile.insert(7, "GKA2137");
//    indexedFile.insert(0, "GD42069");

     //indexedFile.insert(10, "GSL2140");
     //indexedFile.insert(11, "GSL2141");
}