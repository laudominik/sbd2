#pragma once

#include <chrono>
#include <vector>
#include <thread>
#include <fstream>
#include <ios>

#include <time/DiskClocks.h>

namespace sbd::time {
	class Measurement {
	public:
		Measurement(std::ostream& in) : in(in), startTimestamp({ writeClock().get(), readClock().get()}) {};
		~Measurement() {
			auto writes = writeClock().get() - startTimestamp.write;
			auto reads = readClock().get() - startTimestamp.read;
			in << "[Measurement] r: " << reads << " w: " << writes << " io(r+w): " << writes + reads << std::endl;
		}
	private:
		std::ostream& in;
		struct Timestamp {
			uint64_t write, read;
		} startTimestamp;
	};
}
