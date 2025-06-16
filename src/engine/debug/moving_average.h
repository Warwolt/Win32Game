#pragma once

#include <stdint.h>
#include <vector>

namespace engine {

	class MovingAverage {
	public:
		MovingAverage() = default;
		static MovingAverage with_sample_size(int32_t sample_size);

		void add(float value);
		float average() const;

	private:
		int32_t m_index = 0;
		int32_t m_sample_size = 0;
		std::vector<float> m_values;
	};

} // namespace engine
