#pragma once

#include <engine/debug/renderer_test_screen.h>

namespace engine {

	struct InputDevices;
	class Renderer;

	struct DebugState {
		RendererTestScreen renderer_test_screen;
	};

	struct EngineState {
		DebugState debug;
	};

	void update(EngineState* engine, const InputDevices& input);
	void draw(Renderer* renderer, const EngineState& game);

} // namespace engine
