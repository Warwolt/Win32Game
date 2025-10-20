#pragma once

#include <engine/input/time.h>

#include <expected>
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
		AnimationID add_animation(std::vector<AnimationFrame<T>> animation) {
            if (animation.empty()) {
                return INVALID_ANIMATION_ID;
            }

			AnimationID id = AnimationID(m_next_id++);
			// add animation here
			return id;
		}

		std::expected<void, AnimationError> start_animation(AnimationID animation_id, AnimationEntityID entity_id, Time /*now*/) {
			if (animation_id == INVALID_ANIMATION_ID) {
				return std::unexpected(AnimationError::InvalidAnimationID);
			}
			if (entity_id == INVALID_ANIMATION_ENTITY_ID) {
				return std::unexpected(AnimationError::InvalidAnimationEntityID);
			}

			return {};
		}

		void update(Time /*now*/) {
		}

		const T& current_frame(AnimationEntityID /*entity_id*/) const {
			return m_default;
		}

	private:
		int m_next_id = 1;
		T m_default = {};
	};

} // namespace engine
