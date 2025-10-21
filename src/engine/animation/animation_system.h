#pragma once

#include <engine/input/time.h>

#include <expected>
#include <unordered_map>
#include <vector>

namespace engine {

	struct AnimationID {
		int value;
		bool operator==(const AnimationID& rhs) const = default;
	};

	struct AnimationEntityID {
		int value;
		bool operator==(const AnimationEntityID& rhs) const = default;
	};

	constexpr AnimationID INVALID_ANIMATION_ID = AnimationID(0);
	constexpr AnimationEntityID INVALID_ANIMATION_ENTITY_ID = AnimationEntityID(0);

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
	};

	template <typename T>
	class AnimationSystem {
	public:
		AnimationID add_animation(std::vector<AnimationFrame<T>> frames, AnimationOptions options = {}) {
			if (frames.empty()) {
				return INVALID_ANIMATION_ID;
			}

			AnimationID id = AnimationID(m_next_id++);
			m_animations[id.value].frames = frames;
			m_animations[id.value].options = options;

			return id;
		}

		[[nodiscard]] std::expected<void, AnimationError> start_animation(AnimationID animation_id, AnimationEntityID entity_id, Time now) {
			/* Check arguments */
			const bool is_missing_animation = m_animations.find(animation_id.value) == m_animations.end();
			if (animation_id == INVALID_ANIMATION_ID || is_missing_animation) {
				return std::unexpected(AnimationError::InvalidAnimationID);
			}
			if (entity_id == INVALID_ANIMATION_ENTITY_ID) {
				return std::unexpected(AnimationError::InvalidAnimationEntityID);
			}

			/* Add playback */
			m_playback[entity_id.value] = Playback {
				.animation_id = animation_id,
				.start = now,
				.current_frame = 0,
			};

			return {};
		}

		void update(Time now) {
			/* For each playing animation */
			for (auto& [entity_id, playback] : m_playback) {
				/* Determine current frame */
				const Animation& animation = m_animations[playback.animation_id.value];
				const Time elapsed_time = now - playback.start;
				Time prev_durations = 0ms;
				for (int i = 0; i < animation.frames.size(); i++) {
					const AnimationFrame<T>& frame = animation.frames[i];
					if (elapsed_time < prev_durations + frame.duration) {
						playback.current_frame = i;
						break;
					}
					prev_durations += frame.duration;
				}
			}
		}

		const T& current_frame(AnimationEntityID entity_id) const {
			auto it = m_playback.find(entity_id.value);
			if (it == m_playback.end()) {
				return m_default;
			}
			const Playback& playback = it->second;
			const Animation& animation = m_animations.at(playback.animation_id.value);
			return animation.frames[playback.current_frame].value;
		}

	private:
		struct Animation {
			std::vector<AnimationFrame<T>> frames;
			AnimationOptions options;
		};
		struct Playback {
			AnimationID animation_id;
			Time start;
			int current_frame;
		};

		int m_next_id = 1;
		T m_default = {};
		std::unordered_map<int, Animation> m_animations; // AnimationID -> Animation
		std::unordered_map<int, Playback> m_playback;    // AnimationEntityID -> Playback
	};

} // namespace engine
