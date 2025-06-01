#pragma once

#include <engine/debug/renderer_test_screen.h>
#include <engine/commands.h>

namespace engine {

	struct InputDevices;
	class Renderer;

	struct DebugState {
		RendererTestScreen renderer_test_screen;
	};

	struct EngineState {
		bool should_quit = false;
		CommandAPI commands;
		DebugState debug;
	};

	void update(EngineState* engine, const InputDevices& input);
	void draw(Renderer* renderer, const EngineState& game);

} // namespace engine
