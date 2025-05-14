#include <engine/audio/audio_player.h>
#include <engine/input/gamepad.h>
#include <engine/input/input.h>
#include <engine/input/keyboard.h>
#include <engine/logging.h>
#include <engine/render/window.h>
#include <game/game.h>

#include <format>
#include <windows.h>

struct ProgramContext {
	bool should_quit;
	engine::Window window;
	engine::InputDevices input;
	engine::AudioPlayer audio;
	game::GameState game;
};

static ProgramContext g_context;

engine::Window initialize_window_or_abort(HINSTANCE instance, WNDPROC wnd_proc) {
	std::expected<engine::Window, engine::WindowError> window_result = engine::initialize_window(instance, wnd_proc);
	if (!window_result.has_value()) {
		std::string message = std::format("Couldn't create window: {}", engine::window_error_to_str(window_result.error()));
		MessageBoxA(0, message.c_str(), "Error", MB_OK | MB_ICONERROR);
		exit(1);
	}
	return window_result.value();
}

void pump_window_messages() {
	MSG message;
	while (PeekMessageA(&message, 0, 0, 0, PM_REMOVE)) {
		TranslateMessage(&message);
		DispatchMessageA(&message);
		if (message.message == WM_QUIT) {
			g_context.should_quit = true;
		}
	}
}

LRESULT CALLBACK on_window_event(
	HWND window,
	UINT message,
	WPARAM w_param,
	LPARAM l_param
) {
	switch (message) {
		case WM_SIZE: {
			if (window == g_context.window.handle) {
				engine::on_window_resized(&g_context.window);
			}
		} break;

		case WM_SYSKEYDOWN:
		case WM_KEYDOWN: {
			uint32_t key_id = (uint32_t)w_param;
			g_context.input.keyboard.on_key_event(key_id, true);
		} break;

		case WM_SYSKEYUP:
		case WM_KEYUP: {
			uint32_t key_id = (uint32_t)w_param;
			g_context.input.keyboard.on_key_event(key_id, false);
		} break;

		case WM_DESTROY: {
			PostQuitMessage(0);
			return 0;
		} break;

		case WM_CLOSE: {
			PostQuitMessage(0);
			return 0;
		} break;

		case WM_PAINT: {
			PAINTSTRUCT paint;
			HDC device_context = BeginPaint(window, &paint);
			{
				game::draw(&g_context.window.bitmap, g_context.game);
				engine::render_window(g_context.window, device_context);
			}
			EndPaint(window, &paint);
		} break;
	}

	return DefWindowProc(window, message, w_param, l_param);
}

int WINAPI WinMain(
	HINSTANCE instance,
	HINSTANCE /*prev_instance*/,
	LPSTR /*command_line*/,
	int /*command_show*/
) {
	engine::initialize_printf();
	engine::initialize_gamepad_support();
	g_context.window = initialize_window_or_abort(instance, on_window_event);
	g_context.audio = engine::initialize_audio_player();

	// Add cowbell audio
	engine::AudioID cowbell;
	{
		// Load wave file
		HANDLE cowbell_file = CreateFileA(
			"assets/audio/808_cowbell.wav",
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			0,
			NULL
		);

		if (cowbell_file == INVALID_HANDLE_VALUE) {
			fprintf(stderr, "couldn't load wave file!\n");
			exit(1);
		}

		cowbell = g_context.audio.add_audio_from_file(cowbell_file);

		CloseHandle(cowbell_file);
	}

	/* Main loop */
	while (!g_context.should_quit) {
		/* Input */
		pump_window_messages();
		engine::update_input_devices(&g_context.input);

		/* Update */
		game::update(&g_context.game, g_context.input);
		if (g_context.input.keyboard.key_was_pressed_now(VK_ESCAPE)) {
			g_context.should_quit = true;
		}

		// trigger sound with keyboard
		if (g_context.input.keyboard.key_was_pressed_now('1')) {
			g_context.audio.play(cowbell);
		}

		/* Render */
		HDC device_context = GetDC(g_context.window.handle);
		game::draw(&g_context.window.bitmap, g_context.game);
		engine::render_window(g_context.window, device_context);
		ReleaseDC(g_context.window.handle, device_context);
	}

	return 0;
}
