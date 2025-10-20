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
	};

} // namespace engine
