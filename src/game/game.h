#pragma once

namespace engine {
    struct InputDevices;
    struct Bitmap;
} // namespace engine

namespace game {

	struct GameState {
		int x_offset;
		int y_offset;
	};

	void update(GameState* game, const engine::InputDevices& input);
    void draw(engine::Bitmap* bitmap, const GameState& game);

} // namespace game
