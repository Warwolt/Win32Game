#pragma once

#include <functional>

namespace engine {

	// Identifies an animation, e.g. a walk animation.
	// Each animation can be played back for multiple entities.
	struct AnimationID {
		using value_type = int;
		int value;
		bool operator==(const AnimationID& rhs) const = default;
	};

	// Identifies something animated, e.g. a character walking.
	// Multiple entities can use the same animation, but with different playback.
	struct AnimationEntityID {
		int value;
		bool operator==(const AnimationEntityID& rhs) const = default;
	};

	constexpr AnimationID INVALID_ANIMATION_ID = AnimationID(0);
	constexpr AnimationEntityID INVALID_ANIMATION_ENTITY_ID = AnimationEntityID(0);

}

namespace std {

	template <>
	struct hash<engine::AnimationID> {
		size_t operator()(const engine::AnimationID& id) const noexcept {
			return std::hash<int> {}(id.value);
		}
	};

	template <>
	struct hash<engine::AnimationEntityID> {
		size_t operator()(const engine::AnimationEntityID& id) const noexcept {
			return std::hash<int> {}(id.value);
		}
	};

} // namespace std
