#pragma once

#include <engine/animation/animation_id.h>
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

		[[nodiscard]] std::expected<void, AnimationError> start_animation(AnimationEntityID entity_id, AnimationID animation_id, Time now) {
			/* Check arguments */
			const bool is_missing_animation = m_animations.find(animation_id) == m_animations.end();
			if (animation_id == INVALID_ANIMATION_ID || is_missing_animation) {
				return std::unexpected(AnimationError::InvalidAnimationID);
			}
			if (entity_id == INVALID_ANIMATION_ENTITY_ID) {
				return std::unexpected(AnimationError::InvalidAnimationEntityID);
			}

			/* Add playback */
			Playback& playback = m_playing_animations[entity_id];
			playback.animation_id = animation_id;
			playback.start = now;
			m_stopped_animations.erase(entity_id);

			return {};
		}

		void stop_animation(AnimationEntityID entity_id) {
			if (auto it = m_playing_animations.find(entity_id); it != m_playing_animations.end()) {
				m_stopped_animations[it->first] = it->second;
				m_playing_animations.erase(it);
			}
		}

		[[nodiscard]] std::expected<void, AnimationError> restart_animation(AnimationEntityID entity_id, AnimationID animation_id, Time now) {
			stop_animation(entity_id);
			std::expected<void, AnimationError> result = start_animation(entity_id, animation_id, now);
			if (result) {
				Playback& playback = m_playing_animations[entity_id];
				playback.current_frame = 0;
			}
			return result;
		}

		void update(Time now) {
			/* Determine current frame of each playing animation */
			for (auto& [entity_id, playback] : m_playing_animations) {
				const Animation& animation = m_animations[playback.animation_id];
				const Time playback_position = animation.options.looping ? (now - playback.start) % animation.total_length : now - playback.start;
				Time elapsed_time = 0ms;
				for (int i = 0; i < animation.frames.size(); i++) {
					const AnimationFrame<T>& frame = animation.frames[i];
					const bool is_current_frame = playback_position < elapsed_time + frame.duration;
					const bool has_passed_last_frame = i + 1 == (int)animation.frames.size();
					if (is_current_frame || has_passed_last_frame) {
						playback.current_frame = i;
						break;
					}
					elapsed_time += frame.duration;
				}
			}
		}

		const T& current_frame(AnimationEntityID entity_id) const {
			/* Try find frame of playing animation */
			if (auto it = m_playing_animations.find(entity_id); it != m_playing_animations.end()) {
				const Playback& playback = it->second;
				const Animation& animation = m_animations.at(playback.animation_id);
				return animation.frames[playback.current_frame].value;
			}

			/* Try find frame of stopped animation */
			if (auto it = m_stopped_animations.find(entity_id); it != m_stopped_animations.end()) {
				const Playback& playback = it->second;
				const Animation& animation = m_animations.at(playback.animation_id);
				return animation.frames[playback.current_frame].value;
			}

			/* Fall back to default value */
			return m_default;
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
		std::unordered_map<AnimationEntityID, Playback> m_playing_animations;
		std::unordered_map<AnimationEntityID, Playback> m_stopped_animations;
	};

} // namespace engine
