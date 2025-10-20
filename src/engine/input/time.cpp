#include "time.h"
#include <engine/input/time.h>


namespace engine {

	Time Time::now() {
		return Time { std::chrono::high_resolution_clock::now().time_since_epoch() };
	}

	std::chrono::nanoseconds operator-(Time lhs, const Time& rhs) {
		return lhs.value - rhs.value;
	}

	int64_t Time::in_seconds() const {
		return std::chrono::duration_cast<std::chrono::seconds>(value).count();
	}

	int64_t Time::in_milliseconds() const {
		return std::chrono::duration_cast<std::chrono::milliseconds>(value).count();
	}

} // namespace engine
