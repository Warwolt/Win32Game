#include <engine/input/time.h>

namespace engine {

	Time Time::now() {
		return Time { std::chrono::high_resolution_clock::now().time_since_epoch() };
	}

	Time& Time::operator+=(const Time& rhs) {
		this->value += rhs.value;
		return *this;
	}

	Time& Time::operator-=(const Time& rhs) {
		this->value -= rhs.value;
		return *this;
	}

	Time operator+(Time lhs, const Time& rhs) {
		return lhs.value + rhs.value;
	}

	Time operator-(Time lhs, const Time& rhs) {
		return lhs.value - rhs.value;
	}

	int64_t operator/(Time lhs, const Time& rhs) {
		return lhs.value / rhs.value;
	}

} // namespace engine
