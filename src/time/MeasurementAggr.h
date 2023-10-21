#pragma once

#include <ostream>
#include <map>
#include <vector>
#include <iostream>

#include <time/DiskClocks.h>
#include <cli/Choice.h>

namespace sbd::time {
    class MeasurementAggr {

    public:
        void addRead(Operation op, uint64_t read){
            reads[op].push_back(read);
        }

        void addWrite(Operation op, uint64_t write){
            writes[op].push_back(write);
        }

        double avgReads(Operation op){
            return avg(reads[op]);
        }

        double avgWrites(Operation op){
            return avg(writes[op]);
        }

        void printReport(){
            std::cout << "$$$$$$$REPORTS$$$$$$$" << std::endl;
            std::cout << "INSERT    : reads.avg " << avgReads(INSERT) << " writes.avg " << avgWrites(INSERT) << std::endl;
            std::cout << "UPDATE    : reads.avg " << avgReads(UPDATE) << " writes.avg " << avgWrites(UPDATE) << std::endl;
            std::cout << "REMOVE    : reads.avg " << avgReads(REMOVE) << " writes.avg " << avgWrites(REMOVE) << std::endl;
            std::cout << "REORGANISE: reads.avg " << avgReads(REORGANISE) << " writes.avg " << avgWrites(REORGANISE) << std::endl;
        }

    private:
        std::map<Operation, std::vector<uint64_t>> reads;
        std::map<Operation, std::vector<uint64_t>> writes;
        double avg(const std::vector<uint64_t>& meass){
            if(meass.empty()) return 0.f;
            uint64_t sum{};
            for(const auto& meas : meass){
                sum += meas;
            }
            return static_cast<double>(sum) / static_cast<double>(meass.size());
        }

    };


//    class MeasurementAggr {
//    public:
//        MeasurementAggr(std::ostream& in) : in(in), startTimestamp({ writeClock().get(), readClock().get()}) {};
//        ~Measurement() {
//            auto writes = writeClock().get() - startTimestamp.write;
//            auto reads = readClock().get() - startTimestamp.read;
//            in << "[Measurement] r: " << reads << " w: " << writes << " io(r+w): " << writes + reads << std::endl;
//        }
//    private:
//        std::ostream& in;
//        struct Timestamp {
//            uint64_t write, read;
//        } startTimestamp;
//    };
}
