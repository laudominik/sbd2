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
		void tick() { counter++; };
		uint64_t get() { return counter; };
	protected:
		uint64_t counter{0u};
	};
}

