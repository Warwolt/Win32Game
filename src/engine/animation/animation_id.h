#pragma once

#include <functional>

namespace engine {

	struct AnimationID {
		int value;
		bool operator==(const AnimationID& rhs) const = default;
	};

	constexpr AnimationID INVALID_ANIMATION_ID = AnimationID(0);

}

namespace std {

	template <>
	struct hash<engine::AnimationID> {
		size_t operator()(const engine::AnimationID& id) const noexcept {
			return std::hash<int> {}(id.value);
		}
	};

} // namespace std
