#include <engine/debug/assert.h>

#include <utility>

namespace engine {

	void debug_exit() {
		if constexpr (DEBUG_BUILD) {
			std::exit(1);
		}
	}

} // namespace engine