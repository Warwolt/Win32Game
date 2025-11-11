#include <application.h>

#include <engine/debug/logging.h>
#include <engine/debug/profiling.h>
#include <engine/engine.h>
#include <game/game.h>

// temporary
#include <game/scene/gameplay_scene.h>
#include <game/scene/menu_scene.h>

#include <windows.h>
#include <windowsx.h>

#include <format>

#ifdef TRACY_ENABLE
constexpr bool PROFILING_IS_ENABLED = true;
#else
constexpr bool PROFILING_IS_ENABLED = false;
#endif

struct Application {
	engine::Engine engine;
	game::GameData game;
};

static void pump_window_messages(Application* app) {
	CPUProfilingScope_Application();
	MSG message;
	while (PeekMessageA(&message, 0, 0, 0, PM_REMOVE)) {
		TranslateMessage(&message);
		DispatchMessageA(&message);
		if (message.message == WM_QUIT) {
			app->engine.should_quit = true;
		}
	}
}

static void update_input(Application* app) {
	CPUProfilingScope_Application();
	engine::update_input(&app->engine.input, app->engine.input_events, app->engine.window);
	app->engine.input_events = {};

	// HACK: we're not forwarding key events to DefWindowProc, so we have to handle ALT+F4 manually
	const engine::Keyboard& keyboard = app->engine.input.keyboard;
	if (keyboard.key_is_pressed(VK_MENU) && keyboard.key_was_pressed_now(VK_F4)) {
		app->engine.should_quit = true;
	}
}

LRESULT CALLBACK on_window_event(
	Application* app,
	HWND window,
	UINT message,
	WPARAM w_param,
	LPARAM l_param
) {
	CPUProfilingScope_Application();
	switch (message) {
		case WM_SIZE: {
			engine::IVec2 window_size = app->engine.window.on_resized();
		} break;

		case WM_SYSCHAR:
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN: {
			uint32_t key_id = (uint32_t)w_param;
			app->engine.input.keyboard.on_key_event(key_id, true);
			return (LRESULT)0;
		} break;

		case WM_SYSKEYUP:
		case WM_KEYUP: {
			uint32_t key_id = (uint32_t)w_param;
			app->engine.input.keyboard.on_key_event(key_id, false);
			return (LRESULT)0;
		} break;

		case WM_DESTROY: {
			PostQuitMessage(0);
			return 0;
		} break;

		case WM_CLOSE: {
			PostQuitMessage(0);
			return 0;
		} break;

		case WM_MOUSEWHEEL: {
			app->engine.input_events.mouse.mouse_wheel_delta += GET_WHEEL_DELTA_WPARAM(w_param) / WHEEL_DELTA;
		} break;

		case WM_MOUSEMOVE: {
			app->engine.input_events.mouse.mouse_x = (int16_t)GET_X_LPARAM(l_param);
			app->engine.input_events.mouse.mouse_y = (int16_t)GET_Y_LPARAM(l_param);
		} break;

		case WM_ACTIVATE: {
			app->engine.window.on_focus_changed(LOWORD(w_param) != WA_INACTIVE);
		} break;

		case WM_PAINT: {
			/* Input */
			update_input(app);

			/* Update */
			engine::CommandList commands;
			game::update(&app->game, app->engine.input, &commands);
			engine::update(&app->engine, &commands);

			/* Render*/
			game::draw(&app->engine.renderer, app->game);
			engine::draw(&app->engine);
			app->engine.renderer.render(&app->engine.resources);
			app->engine.window.render_wm_paint(app->engine.renderer.bitmap());
		} break;
	}
	return DefWindowProc(window, message, w_param, l_param);
}

void on_dll_unload(Application* application) {
	application->engine.scene_manager.HOT_RELOAD_unregister_all_scenes();
	application->engine.screen_stack.HOT_RELOAD_unregister_all_screens();
}

void on_dll_reloaded(Application* application) {
	game::register_scenes(&application->engine.scene_manager);
	game::register_screens(&application->engine.screen_stack);
	application->engine.scene_manager.HOT_RELOAD_patch_vtables();
	application->engine.screen_stack.HOT_RELOAD_patch_vtables();
}

Application* initialize_application(int argc, char** argv, HINSTANCE instance, WNDPROC on_window_event) {
	Application* application = new Application {};
	engine::CommandList init_commands;

	/* Initialize game */
	application->game = game::initialize(&init_commands);

	/* Initialize engine */
	std::vector<std::string> args = std::vector<std::string>(argv, argv + argc);
	std::optional<engine::Engine> engine = engine::initialize(args, instance, on_window_event, &application->game);
	if (!engine) {
		MessageBoxA(0, "Failed to initialize engine", "Error", MB_OK | MB_ICONERROR);
		exit(1);
	}
	application->engine = std::move(engine.value());

	// Initialize game part 2, fixme move all game init to after engine init
	std::string main_scene = game::register_scenes(&application->engine.scene_manager);
	init_commands.load_scene(main_scene);
	game::register_screens(&application->engine.screen_stack);

	/* Run initial commands */
	init_commands.run_commands(&application->engine);
	LOG_INFO("Initialized");
	LOG_INFO(PROFILING_IS_ENABLED ? "CPU profiling is enabled" : "CPU profiling is disabled");

	return application;
}

bool update_application(Application* app) {
	app->engine.frame_timer.start();
	{
		/* Input */
		pump_window_messages(app);
		update_input(app);

		/* Update */
		engine::CommandList commands;
		game::update(&app->game, app->engine.input, &commands);
		engine::update(&app->engine, &commands);

		/* Draw */
		game::draw(&app->engine.renderer, app->game);
		engine::draw(&app->engine);

		/* Render */
		app->engine.renderer.render(&app->engine.resources);
		app->engine.window.render(app->engine.renderer.bitmap());

		/* Yield CPU to not stall OS */
		Sleep(1);
	}
	app->engine.frame_timer.end();
	CPUProfilingEndFrame();

	return app->engine.should_quit;
}

void shutdown_application(Application* app) {
	LOG_INFO("Shutting down");
	delete app;
}
