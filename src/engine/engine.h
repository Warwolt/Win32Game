#pragma once

#include <engine/audio/audio_player.h>
#include <engine/commands.h>
#include <engine/debug/delta_timer.h>
#include <engine/debug/test_screen/test_screen.h>
#include <engine/file/resource_manager.h>
#include <engine/graphics/renderer.h>
#include <engine/graphics/window.h>
#include <engine/input/input.h>
#include <engine/scene/scene_manager.h>

#include <optional>
#include <string>
#include <vector>

namespace engine {

	struct Input;
	class Renderer;

	struct Engine {
		// application
		bool should_quit = false;
		std::vector<Command> commands;
		// input/output
		InputEvents input_events;
		Input input;
		AudioPlayer audio;
		// file
		ResourceManager resources;
		// graphics
		Renderer renderer;
		Window window;
		Bitmap bitmap;
		// scene
		SceneManager scene_manager;
		SceneID test_scene_id;
		// debug
		DeltaTimer frame_timer;
	};

	std::optional<Engine> initialize(const std::vector<std::string>& args, HINSTANCE instance, WNDPROC wnd_proc);
	void update(Engine* engine);
	void draw(Engine* engine);

} // namespace engine
