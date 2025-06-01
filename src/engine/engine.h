#pragma once

#include <engine/audio/audio_player.h>
#include <engine/commands.h>
#include <engine/debug/debug.h>
#include <engine/graphics/renderer.h>
#include <engine/graphics/window.h>
#include <engine/input/input.h>

namespace engine {

	struct InputDevices;
	class Renderer;

	struct EngineState {
		bool should_quit = false;
		// input output
		InputEvents events;
		InputDevices input;
		CommandAPI commands;
		AudioPlayer audio;
		// graphics
		Renderer renderer;
		Window window;
		// debug
		DebugState debug;
	};

	void update(EngineState* engine, const InputDevices& input);
	void draw(Renderer* renderer, const EngineState& engine);

} // namespace engine
