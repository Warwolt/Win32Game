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

	enum class AnimationError {
		InvalidAnimationID,
		InvalidAnimationEntityID,
	};

	template <typename T>
	class AnimationSystem {
	public:
		AnimationID add_animation(std::vector<AnimationFrame<T>> animation_frames) {
			if (animation_frames.empty()) {
				return INVALID_ANIMATION_ID;
			}

			AnimationID id = AnimationID(m_next_id++);
			m_animations[id.value].frames = animation_frames;

			return id;
		}

		std::expected<void, AnimationError> start_animation(AnimationID animation_id, AnimationEntityID entity_id, Time now) {
			const bool missing_animation = m_animations.find(animation_id.value) == m_animations.end();
			if (animation_id == INVALID_ANIMATION_ID || missing_animation) {
				return std::unexpected(AnimationError::InvalidAnimationID);
			}

			if (entity_id == INVALID_ANIMATION_ENTITY_ID) {
				return std::unexpected(AnimationError::InvalidAnimationEntityID);
			}

			m_playback[entity_id.value] = Playback {
				.animation_id = animation_id,
				.start = now,
			};

			return {};
		}

		void update(Time /*now*/) {
		}

		const T& current_frame(AnimationEntityID entity_id) const {
			auto it = m_playback.find(entity_id.value);
			if (it == m_playback.end()) {
				return m_default;
			}
			const Playback& playback = it->second;
			return m_animations.at(playback.animation_id.value).frames[0].value;
		}

	private:
		struct Animation {
			std::vector<AnimationFrame<T>> frames;
		};
		struct Playback {
			AnimationID animation_id;
			Time start;
		};

		int m_next_id = 1;
		T m_default = {};
		std::unordered_map<int, Animation> m_animations; // AnimationID -> Animation
		std::unordered_map<int, Playback> m_playback;    // AnimationEntityID -> Playback
	};

} // namespace engine
