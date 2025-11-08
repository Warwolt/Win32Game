#include <game/direction.h>

#include <engine/math/vec2.h>

namespace game {

	struct GameData {
		engine::Vec2 player_position;
		Direction player_direction = Direction::Down;
	};

} // namespace game
