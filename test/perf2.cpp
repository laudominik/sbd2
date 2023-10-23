#include <gtest/gtest.h>

#include <time/MeasurementAggr.h>
#include <time/Measurement.h>
#include <impl/IndexedFile.h>
#include <cli/RandomAgent.h>


using namespace sbd;

class Experiment : public testing::Test {
public:
    Experiment(){
        // we want deterministic generation to be able to reproduce the experiment's outcome
        srand(0);
    }
    void generateInserts(size_t n){
        for(auto i=0u; i < n; i++){
            time::Measurement meas(std::cout, aggr, INSERT);
            indexedFile.insert(rand()%constants::MAX_RECORD_KEY, generateCarNumber());
        }
    }

protected:
    impl::IndexedFile indexedFile;
    time::MeasurementAggr aggr;
};


TEST_F(Experiment, sizesTest){
    std::cout << "[INFO] alpha=" << constants::REORGANISATION_ALPHA << std::endl;
    generateInserts(1000);
    indexedFile.blockPrint(std::cout);
    indexedFile.inorderPrint(std::cout);
    aggr.printReport();
    indexedFile.reportSize(std::cout);
}

TEST_F(Experiment, insertPerfTest){

}
