#pragma once

#include <chrono>
#include <stdint.h>

namespace engine {

	using namespace std::chrono_literals;

	struct Time {
		std::chrono::milliseconds value = std::chrono::milliseconds(0);

		static Time now(); // since epoch
		Time() = default;
		Time(std::chrono::nanoseconds ns);
		Time(std::chrono::milliseconds ms);
		Time(std::chrono::seconds sec);

		float in_seconds() const;

		auto operator<=>(const Time&) const = default;
		Time& operator+=(const Time& rhs);
		Time& operator-=(const Time& rhs);
		friend Time operator+(Time lhs, const Time& rhs);
		friend Time operator-(Time lhs, const Time& rhs);
		friend int64_t operator/(Time lhs, const Time& rhs);
		friend Time operator%(Time lhs, const Time& rhs);
	};

} // namespace engine
