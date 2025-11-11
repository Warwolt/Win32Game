#pragma once

#include <engine/commands.h>
#include <engine/debug/delta_timer.h>
#include <engine/file/resource_manager.h>
#include <engine/graphics/renderer.h>
#include <engine/graphics/window.h>
#include <engine/input/input.h>
#include <engine/scene/scene_manager.h>
#include <engine/ui/screen_stack.h>

#include <optional>
#include <string>
#include <vector>

namespace game {
	struct GameData;
}

namespace engine {

	struct Input;
	class Renderer;

	struct Engine {
		// application
		bool should_quit = false;
		game::GameData* game_data = nullptr;

		// input/output
		InputEvents input_events;
		Input input;

		// file
		ResourceManager resources;

		// graphics
		Renderer renderer;
		Window window;

		// scene
		SceneManager scene_manager;

		// user interface
		ScreenStack screen_stack;

		// debug
		DeltaTimer frame_timer;
	};

	std::optional<Engine> initialize(const std::vector<std::string>& args, HINSTANCE instance, WNDPROC wnd_proc, game::GameData* game_data);
	void update(Engine* engine, CommandList* commands);
	void draw(Engine* engine);

} // namespace engine
