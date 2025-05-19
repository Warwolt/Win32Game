#pragma once

#include <engine/math/ivec2.h>

namespace engine {
    struct InputDevices;
    class Renderer;
} // namespace engine

namespace game {

	struct GameState {
		int32_t window_width;
		int32_t window_height;
		int16_t mouse_x;
		int16_t mouse_y;
	};

	void update(GameState* game, const engine::InputDevices& input);
    void draw(engine::Renderer* renderer, const GameState& game);

} // namespace game
