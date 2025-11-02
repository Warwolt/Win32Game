#pragma once

#include <functional>

namespace engine {

	struct EntityID {
		int value;
		bool operator==(const EntityID& rhs) const = default;
	};

	constexpr EntityID INVALID_ENTITY_ID = EntityID(0);

}

namespace std {

	template <>
	struct hash<engine::EntityID> {
		size_t operator()(const engine::EntityID& id) const noexcept {
			return std::hash<int> {}(id.value);
		}
	};

} // namespace std
