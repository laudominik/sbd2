#pragma once

#include <ostream>

#include <time/DiskClocks.h>

namespace sbd::time {
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
