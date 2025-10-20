#pragma once

#include <chrono>
#include <stdint.h>

namespace engine {

	using namespace std::chrono_literals;

	struct Time {
		std::chrono::nanoseconds value = std::chrono::nanoseconds(0);

		static Time now(); // since epoch
		Time() = default;
		Time(std::chrono::nanoseconds ns) : value(ns) {}

		friend std::chrono::nanoseconds operator-(Time lhs, const Time& rhs);
		int64_t in_seconds() const;
		int64_t in_milliseconds() const;

		operator std::chrono::milliseconds() const {
			return std::chrono::duration_cast<std::chrono::milliseconds>(value);
		}
	};

} // namespace engine
