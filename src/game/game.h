#pragma once

#include <engine/audio/audio_id.h>

namespace engine {
	struct EngineState;
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

	GameState initialize(engine::EngineState* engine);
	void update(GameState* game, engine::CommandAPI* commands, const engine::InputDevices& input);
	void draw(engine::Renderer* renderer, const GameState& game);

} // namespace game
