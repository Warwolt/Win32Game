#pragma once

// TODO: move this and DebugState into debug.h
#include <engine/debug/renderer_test_screen.h>

#include <engine/audio/audio_player.h>
#include <engine/commands.h>

namespace engine {

	struct InputDevices;
	class Renderer;

	struct DebugState {
		RendererTestScreen renderer_test_screen;
	};

	struct EngineState {
		bool should_quit = false;
		AudioPlayer audio;
		CommandAPI commands;
		DebugState debug;
	};

	void update(EngineState* engine, const InputDevices& input);
	void draw(Renderer* renderer, const EngineState& game);

} // namespace engine
