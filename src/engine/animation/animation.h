#pragma once

#include <engine/animation/animation_id.h>
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
		IndexOutOfBounds,
	};

	template <typename T>
	struct Animation {
		std::vector<AnimationFrame<T>> frames;
		AnimationOptions options;
	};

	template <typename T>
	class AnimationLibrary {
	public:
		AnimationID add_animation(std::vector<AnimationFrame<T>> frames, AnimationOptions options = {}) {
			AnimationID id = AnimationID(m_next_id++);
			m_animations[id] = Animation<T> { frames, options };
			return id;
		}

		const std::unordered_map<AnimationID, Animation<T>>& animations() const {
			return m_animations;
		}

	private:
		int m_next_id = 1;
		std::unordered_map<AnimationID, Animation<T>> m_animations;
	};

	template <typename T>
	class AnimationPlayer {
	public:
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
			m_is_paused = false;
			m_total_duration = 0ms;
			for (const AnimationFrame<T>& frame : animation.frames) {
				m_total_duration += frame.duration;
			}

			return {};
		}

		void pause() {
			m_is_paused = true;
		}

		std::optional<AnimationError> set_frame(const AnimationLibrary<T>& library, Time global_time_now, int frame_index) {
			/* Try get animation */
			auto it = library.animations().find(m_animation_id);
			if (it == library.animations().end()) {
				return AnimationError::MissingAnimationInLibrary;
			}
			const Animation<T>& animation = it->second;

			/* Bounds check*/
			if (frame_index < 0 || frame_index >= animation.frames.size()) {
				return AnimationError::IndexOutOfBounds;
			}

			/* Move animation time line so desired frame starts now */
			Time time_until_frame = 0ms;
			for (int i = 0; i < frame_index && i < animation.frames.size(); i++) {
				time_until_frame += animation.frames[i].duration;
			}
			m_start_time = global_time_now - time_until_frame;
			m_value = animation.frames[frame_index].value;

			return {};
		}

		void update(const AnimationLibrary<T>& library, Time global_now) {
			if (m_is_paused) {
				return;
			}

			if (auto it = library.animations().find(m_animation_id); it != library.animations().end()) {
				const Animation<T>& animation = it->second;
				const Time relative_now = animation.options.looping ? (global_now - m_start_time) % m_total_duration : global_now - m_start_time;

				/* Animation hasn't started yet */
				if (relative_now < 0ms) {
					m_value = animation.frames.front().value;
				}
				/* Animation has finished */
				else if (relative_now >= m_total_duration) {
					m_value = animation.frames.back().value;
				}
				/* Animation is mid-playback */
				else {
					Time prev_frame_durations = 0ms;
					for (const AnimationFrame<T> frame : animation.frames) {
						const Time frame_end = prev_frame_durations + frame.duration;
						if (relative_now < frame_end) {
							m_value = frame.value;
							break;
						}
						prev_frame_durations += frame.duration;
					}
				}
			}
		}

		const T& value() const {
			return m_value;
		}

	private:
		T m_value = {};
		AnimationID m_animation_id = {};
		Time m_start_time = {};
		Time m_total_duration = {};
		bool m_is_paused = false;
	};

} // namespace engine
