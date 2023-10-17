#include <iostream>

#include <time/DiskClocks.h>
#include <impl/IndexedFile.h>


using namespace sbd;

/*
 * TODO:
 * - random generation/from file/from stdin
 * - find bugs
 * - improve number of reads (unnecessary reads/writes in file creation and reorganisation)
 * - experiment
 * - printing file
 */

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
    //indexedFile.insert(3, "GSLB00B");
    indexedFile.insert(0, "VXDDEEE");
    indexedFile.insert(0x55555588, "POGGERS");
    indexedFile.insert(0x06969690, "ABCD");
    indexedFile.insert(0x05555555, "LOLO <3");
//
      indexedFile.insert(5, "XOXO");
      indexedFile.insert(8, "UVU");
      indexedFile.insert(6, "YOYO");
      indexedFile.insert(7, "XDD");
      indexedFile.insert(12, "LOLO1");
      indexedFile.insert(13, "LOLO2");
//
      indexedFile.insert(15, "LOLO3");
      indexedFile.insert(16, "LOLO4");
      indexedFile.insert(17, "LOLO5");
      indexedFile.insert(18, "LOLO6");
      indexedFile.insert(20, "LOLO7");
      indexedFile.insert(19, "LOLO8");

//      auto y = indexedFile.find(3);
//      auto z = indexedFile.find(19);
//      auto t = indexedFile.find(0x55555588);
//      auto p = indexedFile.find(2);
//        indexedFile.remove(19);
//        indexedFile.reorganise();
//        indexedFile.remove(0x55555588);
//        indexedFile.reorganise();
//        indexedFile.remove(0);
//        indexedFile.remove(1);
        indexedFile.remove(2);
        indexedFile.update(0, 0, "UPDATED");
        indexedFile.update(0x05555556,0x05555556, "ALSOHER");
        indexedFile.update(0, 69, "UPDAT69");
        indexedFile.update(18, 70, "LOLO6_2");
//        indexedFile.reorganise();

//        indexedFile.insert(3, "bruuh");
//        indexedFile.remove(2);
        //indexedFile.reorganise();
//        indexedFile.remove(0);
//        indexedFile.remove(1);
//        indexedFile.reorganise();
}