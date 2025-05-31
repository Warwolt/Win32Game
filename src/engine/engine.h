#pragma once

#include <engine/graphics/renderer.h>

namespace engine {

	struct InputDevices;

	struct DebugState {
		RendererTestState renderer_test;
	};

	struct EngineState {
		DebugState debug;
	};

	void update(EngineState* engine, const InputDevices& input);
	void draw(Renderer* renderer, const EngineState& game);

} // namespace engine
