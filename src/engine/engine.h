#pragma once

#include <engine/audio/audio_player.h>
#include <engine/commands.h>
#include <engine/debug/debug.h>
#include <engine/file/resource_manager.h>
#include <engine/graphics/renderer.h>
#include <engine/graphics/window.h>
#include <engine/input/input.h>

#include <expected>
#include <variant>

namespace engine {

	struct InputDevices;
	class Renderer;

	struct Engine {
		// application
		bool should_quit = false;
		IVec2 screen_resolution;
		std::vector<Command> commands;
		// input/output
		InputEvents events;
		InputDevices input;
		AudioPlayer audio;
		// file
		ResourceManager resources;
		// graphics
		Renderer renderer;
		Window window;
		Bitmap bitmap;
		// debug
		DebugState debug;
	};

	using EngineError = std::variant<WindowError>;
	using EngineResult = std::expected<Engine, EngineError>;

	EngineResult initialize(HINSTANCE instance, WNDPROC wnd_proc, const char* window_title);
	void update(Engine* engine, const InputDevices& input);
	void draw(Renderer* renderer, Engine* engine);

} // namespace engine
