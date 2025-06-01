#pragma once

#include <engine/audio/audio_id.h>
#include <engine/math/ivec2.h>


namespace engine {
	struct InputDevices;
	class Renderer;
	class CommandAPI;
} // namespace engine

namespace game {

	struct GameState {
		struct Assets {
			struct Audio {
				engine::AudioID cowbell;
			} audio;
		} assets;
	};

	void update(GameState* game, engine::CommandAPI* commands, const engine::InputDevices& input);
	void draw(engine::Renderer* renderer, const GameState& game);

} // namespace game
