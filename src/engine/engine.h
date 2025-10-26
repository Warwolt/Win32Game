#pragma once

#include <engine/commands.h>
#include <engine/debug/debug.h>
#include <engine/debug/debug_state.h>
#include <engine/file/resource_manager.h>
#include <engine/graphics/renderer.h>
#include <engine/graphics/window.h>
#include <engine/input/input.h>

#include <optional>
#include <string>
#include <vector>

namespace engine {

	struct InputDevices;
	class Renderer;

	struct Engine {
		// application
		bool should_quit = false;
		IVec2 screen_resolution;
		std::vector<Command> commands;
		// input/output
		InputEvents input_events;
		InputDevices input;
		// file
		ResourceManager resources;
		// graphics
		Renderer renderer;
		Window window;
		Bitmap bitmap;
		// debug
		DebugState debug;
	};

	std::optional<Engine> initialize(const std::vector<std::string>& args, HINSTANCE instance, WNDPROC wnd_proc);
	void update(Engine* engine);
	void draw(Engine* engine);

} // namespace engine
