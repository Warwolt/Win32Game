#include <engine/debug/delta_timer.h>

namespace engine {

	void DeltaTimer::start() {
		m_start = std::chrono::high_resolution_clock::now();
	}

	void DeltaTimer::end() {
        float delta = std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - m_start).count();
        m_deltas.push_back(delta);
	}

	float DeltaTimer::average_delta() const {
        return m_deltas.average();
	}

} // namespace engine
