#pragma once

#include <cstdint>

#define DECLARE_CLOCK(name) \
	inline Clock& name () { \
		static Clock clock; \
		return clock; \
	}

namespace sbd::time {
	class Clock {
	public:
		void tick() { if(!frozen) counter++; };
		uint64_t get() { return counter; }
        void freeze() { frozen = true; }
        void unfreeze() { frozen = false; }
	protected:
        bool frozen = false;
		uint64_t counter{0u};
	};
}

