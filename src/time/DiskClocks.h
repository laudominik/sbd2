#pragma once

#include "Clock.h"

namespace sbd::time {
	DECLARE_CLOCK(writeClock);
	DECLARE_CLOCK(readClock);
	DECLARE_CLOCK(phaseClock);
}