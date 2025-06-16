#include <engine/math/moving_average.h>

#include <numeric>

namespace engine {

	MovingAverage MovingAverage::with_sample_size(int32_t sample_size) {
		MovingAverage average = {};
		average.m_sample_size = sample_size;
		return average;
	}

	void MovingAverage::push_back(float value) {
		if (m_index < m_sample_size) {
			m_values.push_back(value);
		}
		else {
			m_values[m_index % m_sample_size] = value;
		}
		m_index++;
	}

	float MovingAverage::average() const {
		if (m_values.empty()) {
			return 0.0f;
		}
		return std::accumulate(m_values.begin(), m_values.end(), 0.0f) / m_values.size();
	}

} // namespace engine
