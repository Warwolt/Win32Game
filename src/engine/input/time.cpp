#include "time.h"
#include <engine/input/time.h>


namespace engine {

	Time Time::now() {
		return Time { std::chrono::high_resolution_clock::now().time_since_epoch() };
	}

	std::chrono::nanoseconds operator-(Time lhs, const Time& rhs) {
		return lhs.value - rhs.value;
	}

} // namespace engine
