#pragma once

#include <engine/audio/audio_player.h>
#include <engine/commands.h>
#include <engine/debug/debug.h>

namespace engine {

	struct InputDevices;
	class Renderer;

	struct EngineState {
		bool should_quit = false;
		AudioPlayer audio;
		CommandAPI commands;
		DebugState debug;
	};

	void update(EngineState* engine, const InputDevices& input);
	void draw(Renderer* renderer, const EngineState& engine);

} // namespace engine
