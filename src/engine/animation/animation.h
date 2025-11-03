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
		std::vector<AnimationFrame<T>> frames;
	};

	// Stores Animation instances
	template <typename T>
	class AnimationLibrary {
	public:
		AnimationID add_animation(std::vector<AnimationFrame<T>> frames) {
			AnimationID id = AnimationID(m_next_id++);
			m_animations[id] = Animation { frames };
			return id;
		}

		const std::unordered_map<AnimationID, Animation<T>>& animations() const {
			return m_animations;
		}

	private:
		int m_next_id = 1;
		std::unordered_map<AnimationID, Animation<T>> m_animations;
	};

	// Keeps track of playing back a given animation
	template <typename T>
	class AnimationPlayer {
	public:
		void update(const AnimationLibrary<T>& library, Time global_now) {
			if (auto it = library.animations().find(m_animation_id); it != library.animations().end()) {
				const Animation<T>& animation = it->second;
				const Time relative_now = global_now - m_start_time;
				Time elapsed_time = 0ms;
				for (const AnimationFrame<T> frame : animation.frames) {
					if (relative_now <= elapsed_time) {
						m_value = frame.value;
						break;
					}
					elapsed_time += frame.duration;
				}
			}
		}

		std::optional<AnimationError> play(const AnimationLibrary<T>& library, AnimationID animation_id, Time start_time) {
			/* Try get animation */
			auto it = library.animations().find(animation_id);
			if (it == library.animations().end()) {
				return AnimationError::MissingAnimationInLibrary;
			}
			const Animation<T>& animation = it->second;

			/* Set up animation playback */
			m_value = animation.frames[0].value;
			m_animation_id = animation_id;
			m_start_time = start_time;
			return {};
		}

		// void stop()
		// void set_frame(int index)

		const T& value() const {
			return m_value;
		}

	private:
		T m_value = {};
		AnimationID m_animation_id = {};
		Time m_start_time = {};
	};

} // namespace engine
