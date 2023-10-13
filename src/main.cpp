#include <iostream>

#include <time/DiskClocks.h>
#include <impl/IndexedFile.h>


using namespace sbd;

int main(){
    impl::IndexedFile indexedFile;
    indexedFile.insert(1, "GSL2137");
    indexedFile.insert(0x05555556, "GSL2138");
    indexedFile.insert(0x05555575, "GSL2138");
    indexedFile.insert(0x05555565, "GSL2138");
    indexedFile.insert(9, "GSL2139");
    indexedFile.insert(10, "GSL2222");
    indexedFile.insert(11, "GSL6969");
    indexedFile.insert(2, "GSL0000");
    indexedFile.insert(3, "GSLB00B");
    indexedFile.insert(0x055555588, "POGGERS");
    indexedFile.insert(0x06969690, "ABCD");
    indexedFile.insert(0x05555555, "LOLO <3");

//    indexedFile.insert(6, "GSL6969");
//    indexedFile.insert(7, "GKA2137");
//    indexedFile.insert(0, "GD42069");

     //indexedFile.insert(10, "GSL2140");
     //indexedFile.insert(11, "GSL2141");
}