#pragma once

#include <engine/math/moving_average.h>

#include <chrono>

namespace engine {

	class DeltaTimer {
	public:
		void start();
		void end();
		float average_delta() const; // in seconds

	private:
		std::chrono::steady_clock::time_point m_start;
		MovingAverage m_deltas = MovingAverage::with_sample_size(60);
	};

} // namespace engine
