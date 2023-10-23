#include <iostream>

#include <time/DiskClocks.h>
#include <impl/IndexedFile.h>
#include <cli/Agent.h>
#include <cli/InteractiveAgent.h>
#include <cli/FileAgent.h>
#include <cli/RandomAgent.h>
#include <util/Config.h>
#include <time/Measurement.h>
#include <time/MeasurementAggr.h>

using namespace sbd;

/*
 * TODO:
 * - find bugs
 * - experiment
 * - report
 */

void cli();

int main(){
    cli();
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
        } else if(choice == "FILE") {
            std::cout << "enter filename" << std::endl;
            std::string filename;
            std::cin >> filename;
            agent = std::make_unique<FileAgent>(filename);
            break;
        } else if(choice == "DEBUG"){
            util::Config::instance().switchDebugMode();
        } else {
            std::cout << "invalid command" << std::endl;
        }
    }

    bool running = true;
    time::MeasurementAggr aggr;
    while(running){
        auto choice = agent->makeChoice();
        {
            switch (choice.op) {
                case EXIT:
                    running = false;
                    break;
                case INSERT:{
                    time::Measurement meas(std::cout, aggr, INSERT);
                    indexedFile.insert(choice.uintArgKey, choice.strArg);
                    break;
                }
                case REMOVE:{
                    time::Measurement meas(std::cout, aggr, REMOVE);
                    indexedFile.remove(choice.uintArgKey);
                    break;
                }
                case UPDATE: {
                    time::Measurement meas(std::cout, aggr, UPDATE);
                    indexedFile.update(choice.uintArgKey, choice.uintArgNewKey, choice.strArg);
                    break;
                }
                case INORDER:
                    indexedFile.inorderPrint(std::cout);
                    break;
                case REORGANISE: {
                    time::Measurement meas(std::cout, aggr, REORGANISE);
                    indexedFile.reorganise();
                    break;
                }
            }
            if(util::Config::instance().isDebugModeEnabled()){
                std::cout << indexedFile;
            }
        }
    }
    aggr.printReport();
    indexedFile.reportSize(std::cout);

}