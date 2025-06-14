#include <engine/debug/assert.h>

#include <utility>

namespace engine {

	void debug_exit() {
		if constexpr (IS_DEBUG_BUILD) {
			std::exit(1);
		}
	}

} // namespace engine
