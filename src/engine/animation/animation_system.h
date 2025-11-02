#pragma once

#include <engine/animation/animation_id.h>
#include <engine/entity_id.h>
#include <engine/input/time.h>

#include <expected>
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
		InvalidAnimationID,
		InvalidAnimationEntityID,
		EntityHasNoPlayingAnimation,
		IndexOutOfBounds,
	};

	template <typename T>
	class AnimationSystem {
	public:
		AnimationID add_animation(std::vector<AnimationFrame<T>> frames, AnimationOptions options = {}) {
			if (frames.empty()) {
				return INVALID_ANIMATION_ID;
			}

			AnimationID id = AnimationID(m_next_id++);
			m_animations[id].frames = frames;
			m_animations[id].options = options;
			for (const AnimationFrame<T>& frame : frames) {
				m_animations[id].total_length += frame.duration;
			}

			return id;
		}

	private:
		struct Animation {
			std::vector<AnimationFrame<T>> frames;
			AnimationOptions options;
			Time total_length;
		};
		struct Playback {
			AnimationID animation_id;
			Time start;
			int current_frame;
		};

		int m_next_id = 1;
		T m_default = {};
		std::unordered_map<AnimationID, Animation> m_animations;
		std::unordered_map<EntityID, Playback> m_playing_animations;
		std::unordered_map<EntityID, Playback> m_stopped_animations;
	};

} // namespace engine
