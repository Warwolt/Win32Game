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
		Time(std::chrono::milliseconds ms) : value(std::chrono::duration_cast<std::chrono::nanoseconds>(ms)) {}
		Time(std::chrono::seconds ms) : value(std::chrono::duration_cast<std::chrono::nanoseconds>(ms)) {}

		auto operator<=>(const Time&) const = default;
		Time& operator+=(const Time& rhs);
		Time& operator-=(const Time& rhs);
		friend Time operator+(Time lhs, const Time& rhs);
		friend Time operator-(Time lhs, const Time& rhs);
		friend int64_t operator/(Time lhs, const Time& rhs);
	};

} // namespace engine
