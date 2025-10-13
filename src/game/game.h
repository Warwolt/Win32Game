#pragma once

#include <engine/audio/audio_id.h>
#include <engine/commands.h>

#include <vector>

namespace engine {
	struct Engine;
	struct InputDevices;
	class Renderer;
} // namespace engine

namespace game {

	struct Game {
		struct Assets {
			struct Audio {
				engine::AudioID cowbell;
			} audio;
		} assets;
	};

	Game initialize(engine::Engine* engine);
	void update(Game* game, std::vector<engine::Command>* commands, const engine::InputDevices& input);
	void draw(engine::Renderer* renderer, const Game& game);

} // namespace game
