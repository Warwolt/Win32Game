#pragma once

#include <engine/math/ivec2.h>

namespace engine {
    struct InputDevices;
    class Renderer;
} // namespace engine

namespace game {

	struct GameState {
		bool show_renderer_test_screen;
	};

	void update(GameState* game, const engine::InputDevices& input);
    void draw(engine::Renderer* renderer, const GameState& game);

} // namespace game
