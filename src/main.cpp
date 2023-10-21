#include <iostream>

#include <time/DiskClocks.h>
#include <impl/IndexedFile.h>
#include <cli/Agent.h>
#include <cli/InteractiveAgent.h>
#include <cli/RandomAgent.h>
#include <util/Config.h>
#include <time/Measurement.h>

using namespace sbd;

/*
 * TODO:
 * - from file
 * - find bugs
 * - improve number of reads (unnecessary reads/writes in file creation and reorganisation)
 * - experiment
 * - report
 */

void cli();

int main(){
    cli();
//    impl::IndexedFile indexedFile;
//    indexedFile.insert(1, "GSL2137");
//    indexedFile.insert(0x05555556, "GSL2138");
//    indexedFile.insert(0x05555575, "GSL2138");
//    indexedFile.insert(0x05555565, "GSL2138");
//    indexedFile.insert(9, "GSL2139");
//    indexedFile.insert(10, "GSL2222");
//    indexedFile.insert(11, "GSL6969");
//    indexedFile.insert(2, "GSL0000");
//    //indexedFile.insert(3, "GSLB00B");
//    indexedFile.insert(0, "VXDDEEE");
//    indexedFile.insert(0x55555588, "POGGERS");
//    indexedFile.insert(0x06969690, "ABCD");
//    indexedFile.insert(0x05555555, "LOLO <3");
////
//      indexedFile.insert(5, "XOXO");
//      indexedFile.insert(8, "UVU");
//      indexedFile.insert(6, "YOYO");
//      indexedFile.insert(7, "XDD");
//      indexedFile.insert(12, "LOLO1");
//      indexedFile.insert(13, "LOLO2");
////
//      indexedFile.insert(15, "LOLO3");
//      indexedFile.insert(16, "LOLO4");
//      indexedFile.insert(17, "LOLO5");
//      indexedFile.insert(18, "LOLO6");
//      indexedFile.insert(20, "LOLO7");
//      indexedFile.insert(19, "LOLO8");
//
////      auto y = indexedFile.find(3);
////      auto z = indexedFile.find(19);
////      auto t = indexedFile.find(0x55555588);
////      auto p = indexedFile.find(2);
////        indexedFile.remove(19);
////        indexedFile.reorganise();
////        indexedFile.remove(0x55555588);
////        indexedFile.reorganise();
////        indexedFile.remove(0);
////        indexedFile.remove(1);
//        //indexedFile.remove(2);
//        indexedFile.update(0, 0, "UPDATED");
//        indexedFile.update(0x05555556,0x05555556, "ALSOHER");
//        indexedFile.update(0, 69, "UPDAT69");
//        indexedFile.update(18, 70, "LOLO6_2");

        //std::cout << indexedFile;
//        indexedFile.inorderPrint(std::cout);
//        indexedFile.reorganise();

//        indexedFile.insert(3, "bruuh");
//        indexedFile.remove(2);
        //indexedFile.reorganise();
//        indexedFile.remove(0);
//        indexedFile.remove(1);
//        indexedFile.reorganise();

// INSERT 5 LOLO
// INSERT 2 LOLO2
// INSERT 5 LOLO5
}

void cli(){
    impl::IndexedFile indexedFile;
    std::unique_ptr<Agent> agent;

    while(true){
        std::string choice;
        std::cout << "[INFO] debug mode " << (util::Config::instance().isDebugModeEnabled() ? "true" : "false") << std::endl;
        std::cout << "CHOOSE INPUT TYPE: CLI/FILE/RANDOM, set/unsets DEBUG" << std::endl;
        std::cin >> choice;
        if(choice == "CLI") {
            agent = std::make_unique<InteractiveAgent>();
            break;
        } else if(choice == "RANDOM"){
            std::cout << "how many operations to generate?" << std::endl;
            uint32_t n;
            std::cin >> n;
            agent = std::make_unique<RandomAgent>(n);
            break;
        } else if(choice == "DEBUG"){
            util::Config::instance().switchDebugMode();
        } else {
            std::cout << "invalid command" << std::endl;
        }
    }

    bool running = true;
    while(running){
        auto choice = agent->makeChoice();
        {
            time::Measurement meas(std::cout);
            switch (choice.op) {
                case EXIT:
                    running = false;
                    break;
                case INSERT:
                    indexedFile.insert(choice.uintArgKey, choice.strArg);
                    break;
                case REMOVE:
                    indexedFile.remove(choice.uintArgKey);
                    break;
                case UPDATE:
                    indexedFile.update(choice.uintArgKey, choice.uintArgNewKey, choice.strArg);
                    break;
                case INORDER:
                    indexedFile.inorderPrint(std::cout);
                    break;
                case REORGANISE:
                    indexedFile.reorganise();
                    break;
            }
            if(util::Config::instance().isDebugModeEnabled()){
                std::cout << indexedFile;
            }
        }
    }

}