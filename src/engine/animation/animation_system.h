#pragma once

#include <engine/input/time.h>

#include <vector>

namespace engine {

	template <typename T>
	struct AnimationID {
		int value;
	};

	template <typename T>
	struct AnimationFrame {
		Time duration;
		T value;
	};

	template <typename T>
	class AnimationSystem {
	public:

	private:
	};

} // namespace engine
