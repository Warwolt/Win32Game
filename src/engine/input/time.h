#pragma once

#include <chrono>
#include <stdint.h>

namespace engine {

	struct Time {
		std::chrono::nanoseconds value = std::chrono::nanoseconds(0);

		static Time now(); // since epoch
		friend Time operator-(Time lhs, const Time& rhs);
		int64_t in_seconds() const;
		int64_t in_milliseconds() const;
	};

} // namespace engine
