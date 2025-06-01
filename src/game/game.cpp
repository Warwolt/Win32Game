#include <game/game.h>

#include <engine/audio/audio_player.h>
#include <engine/commands.h>
#include <engine/engine.h>
#include <engine/graphics/renderer.h>
#include <engine/input/input.h>


#include <string>

namespace game {

	// This is a temporary placeholder until we have a real system for loading assets
	// We just exit(1) on error here instead of returning an error since it's just for testing.
	static engine::AudioID load_audio_from_file_or_abort(engine::AudioPlayer* audio, const char* path) {
		HANDLE cowbell_file = CreateFileA(
			path,
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			0,
			NULL
		);
		std::expected<engine::AudioID, std::string> result = audio->add_audio_from_file(cowbell_file);
		CloseHandle(cowbell_file);

		if (!result.has_value()) {
			fprintf(stderr, "Failed to load audio file \"%s\": %s\n", path, result.error().c_str());
			exit(1);
		}

		return result.value();
	}

	GameState initialize(engine::EngineState* engine) {
		GameState game = {};
		game.assets.audio.cowbell = load_audio_from_file_or_abort(&engine->audio, "assets/audio/808_cowbell.wav");
		return game;
	}

	void update(GameState* game, engine::CommandAPI* commands, const engine::InputDevices& input) {
		if (input.keyboard.key_was_pressed_now(VK_ESCAPE)) {
			commands->quit();
		}

		if (input.keyboard.key_was_pressed_now('1')) {
			commands->play_sound(game->assets.audio.cowbell);
		}
	}

	void draw(engine::Renderer* /*renderer*/, const GameState& /*game*/) {
	}

} // namespace game
