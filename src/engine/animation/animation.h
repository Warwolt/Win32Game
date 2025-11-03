#pragma once

#include <engine/animation/animation_id.h>
#include <engine/entity_id.h>
#include <engine/input/time.h>

#include <optional>
#include <unordered_map>
#include <vector>

namespace engine {

	template <typename T>
	struct AnimationFrame {
		T value;
		Time duration;
	};

	struct AnimationOptions {
		bool looping = false;
	};

	enum class AnimationError {
		MissingAnimationInLibrary,
	};

	template <typename T>
	struct Animation {
		// std::vector<AnimationFrame<T>> frames;
	};

	// Stores Animation instances
	template <typename T>
	class AnimationLibrary {
	public:
		// std::unordered_map<AnimationID, Animation> m_animations
	private:
	};

	// Keeps track of playing back a given animation
	template <typename T>
	class AnimationPlayer {
	public:
		std::optional<AnimationError> play(const AnimationLibrary<T>& library, AnimationID animation_id, Time start_time) {
			return AnimationError::MissingAnimationInLibrary;
		}
		// void stop()
		// void set_frame(int index)
		const T* value(const AnimationLibrary<T>& /*library*/) {
			return nullptr;
		}

	private:
	};

} // namespace engine
