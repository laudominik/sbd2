#include <iostream>

#include <time/DiskClocks.h>
#include <impl/IndexedFile.h>


using namespace sbd;

int main(){

    impl::IndexedFile indexedFile;

    std::cout << time::readClock().get() << std::endl;
    std::cout << time::writeClock().get() << std::endl;

}