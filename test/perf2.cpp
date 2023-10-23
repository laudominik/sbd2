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

protected:
    time::MeasurementAggr aggr;
};

void generateInserts(impl::IndexedFile& indexedFile, time::MeasurementAggr& aggr, size_t n){
    for(auto i=0u; i < n; i++){
        time::Measurement meas(std::cout, aggr, INSERT, false);
        indexedFile.insert(rand()%constants::MAX_RECORD_KEY, generateCarNumber());
    }
}


TEST_F(Experiment, sizesAlphaTest){
    std::cout << "[INFO] alpha=" << constants::REORGANISATION_ALPHA << std::endl;
    impl::IndexedFile indexedFile;
    generateInserts(indexedFile, aggr, 1000);
    indexedFile.reportSize(std::cout);
}

TEST_F(Experiment, sizesNTest){
    static constexpr auto Ns = {50, 250, 500, 1000, 2500, 5000, 7500, 10000};
    for(auto N : Ns){
        std::cout << "meas for " << N << std::endl;
        impl::IndexedFile indexedFile_;
        generateInserts(indexedFile_, aggr, N);
        indexedFile_.reportSize(std::cout);
    }
}

TEST_F(Experiment, insertPerfTest){
    impl::IndexedFile indexedFile;
    generateInserts(indexedFile, aggr, 1000);
    aggr.printReport();
}

TEST_F(Experiment, updateExistingTest){

}

TEST_F(Experiment, updateNonExistingTest){

}

TEST_F(Experiment, removeExistingTest){

}

TEST_F(Experiment, removeNonExistingTest){

}