#pragma once

#include <chrono>
#include <vector>
#include <thread>
#include <fstream>
#include <ios>

#include <time/DiskClocks.h>
#include <time/MeasurementAggr.h>

namespace sbd::time {
	class Measurement {
	public:
		Measurement(std::ostream& in, MeasurementAggr& aggr, Operation op) :
            in(in),
            startTimestamp({ writeClock().get(), readClock().get()}),
            aggr(aggr),
            op(op){};
		~Measurement() {
			auto writes = writeClock().get() - startTimestamp.write;
			auto reads = readClock().get() - startTimestamp.read;
			in << "[Measurement] r: " << reads << " w: " << writes << " io(r+w): " << writes + reads << std::endl;
            aggr.addRead(op, reads);
            aggr.addWrite(op, writes);
		}
	private:
		std::ostream& in;
		struct Timestamp {
			uint64_t write, read;
		} startTimestamp;
        MeasurementAggr& aggr;
        Operation op;
	};
}
