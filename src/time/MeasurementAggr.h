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
            std::cout << "INSERT    : reads.avg " << avgReads(INSERT) << " writes.avg " << avgWrites(INSERT) << " io.avg " << avgReads(INSERT) + avgWrites(INSERT) << std::endl;
            std::cout << "UPDATE    : reads.avg " << avgReads(UPDATE) << " writes.avg " << avgWrites(UPDATE) << " io.avg " << avgReads(UPDATE) + avgWrites(UPDATE) << std::endl;
            std::cout << "REMOVE    : reads.avg " << avgReads(REMOVE) << " writes.avg " << avgWrites(REMOVE) << " io.avg " << avgReads(REMOVE) + avgWrites(REMOVE) << std::endl;
            std::cout << "GET       : reads.avg " << avgReads(GET) << " writes.avg " << avgWrites(GET) << " io.avg " << avgReads(GET) + avgWrites(GET) << std::endl;
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
}
