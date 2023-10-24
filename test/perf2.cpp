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

std::vector<uint32_t> generateInserts(impl::IndexedFile& indexedFile, time::MeasurementAggr& aggr, size_t n, uint32_t boundary = constants::MAX_RECORD_KEY){
    std::vector<uint32_t> keys;
    for(auto i=0u; i < n; i++){
        time::Measurement meas(std::cout, aggr, INSERT, false);
        auto key = rand()%boundary;
        keys.push_back(key);
        indexedFile.insert(key, generateCarNumber());
    }
    return keys;
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

TEST_F(Experiment, updateExistingKeyChangeTest){
    impl::IndexedFile indexedFile;
    static constexpr auto numOfOps = 100;
    auto keys = generateInserts(indexedFile, aggr, 1000);
    for(auto i = 0u; i < numOfOps; i++){
        auto key = keys[random()%keys.size()];
        time::Measurement meas(std::cout, aggr, UPDATE, false);
        indexedFile.update(key, random()%constants::MAX_RECORD_KEY, "BLABLA");
    }
    aggr.printReport();
}

TEST_F(Experiment, updateExistingKeySameTest){
    impl::IndexedFile indexedFile;
    static constexpr auto numOfOps = 100;
    auto keys = generateInserts(indexedFile, aggr, 10000);
    for(auto i = 0u; i < numOfOps; i++){
        auto key = keys[random()%keys.size()];
        time::Measurement meas(std::cout, aggr, UPDATE, false);
        indexedFile.update(key, key, "BLABLA");
    }
    aggr.printReport();
    indexedFile.inorderPrint(std::cout);
}


TEST_F(Experiment, updateNonExistingTest){
    impl::IndexedFile indexedFile;
    static constexpr auto numOfOps = 100;
    auto keys = generateInserts(indexedFile, aggr, 1000);
    for(auto i = 0u; i < numOfOps; i++){
        auto key = random()%constants::MAX_RECORD_KEY;
        time::Measurement meas(std::cout, aggr, UPDATE, false);
        indexedFile.update(key, key, "BLABLA");
    }
    aggr.printReport();
}

TEST_F(Experiment, removeExistingTest){
    impl::IndexedFile indexedFile;
    static constexpr auto numOfOps = 100;
    auto keys = generateInserts(indexedFile, aggr, 1000);
    for(auto i = 0u; i < numOfOps; i++){
        auto key = keys[random()%keys.size()];
        time::Measurement meas(std::cout, aggr, REMOVE, false);
        indexedFile.remove(key);
    }
    aggr.printReport();
}

TEST_F(Experiment, removeNonExistingTest){
    impl::IndexedFile indexedFile;
    static constexpr auto numOfOps = 100;
    auto keys = generateInserts(indexedFile, aggr, 1000);
    for(auto i = 0u; i < numOfOps; i++){
        auto key = random()%constants::MAX_RECORD_KEY;
        time::Measurement meas(std::cout, aggr, REMOVE, false);
        indexedFile.remove(key);
    }
    aggr.printReport();
}


