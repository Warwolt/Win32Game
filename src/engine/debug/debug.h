#pragma once

#include <engine/commands.h>
#include <engine/math/ivec2.h>

namespace engine {

	class Renderer;
	class ResourceManager;
	struct InputDevices;
	struct DebugState;

	void initialize_debug(DebugState* debug, ResourceManager* resources);
	void update_debug(DebugState* debug, const InputDevices& input, std::vector<Command>* commands);
	void draw_debug(Renderer* renderer, const DebugState& debug, IVec2 screen_resolution);

} // namespace engine
