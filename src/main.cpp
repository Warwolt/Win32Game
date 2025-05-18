#include <engine/audio/audio_player.h>
#include <engine/debug/logging.h>
#include <engine/graphics/color.h>
#include <engine/graphics/window.h>
#include <engine/input/gamepad.h>
#include <engine/input/input.h>
#include <engine/input/keyboard.h>
#include <engine/input/mouse.h>
#include <engine/math/ivec2.h>
#include <game/game.h>


#include <format>
#include <windows.h>
#include <windowsx.h>

namespace engine {
	struct Assets {
		struct Audio {
			engine::AudioID cowbell;
		} audio;
	};
} // namespace engine

struct ProgramContext {
	bool should_quit;
	engine::MouseEvents mouse_events;
	engine::Window window;
	engine::InputDevices input;
	engine::AudioPlayer audio;
	engine::Assets assets;
	game::GameState game;
};

static ProgramContext g_context;

static engine::Window initialize_window_or_abort(HINSTANCE instance, WNDPROC wnd_proc, const char* window_title) {
	std::expected<engine::Window, engine::WindowError> window_result = engine::initialize_window(instance, wnd_proc, window_title);
	if (!window_result.has_value()) {
		std::string message = std::format("Couldn't create window: {}", engine::window_error_to_str(window_result.error()));
		MessageBoxA(0, message.c_str(), "Error", MB_OK | MB_ICONERROR);
		exit(1);
	}
	return window_result.value();
}

static void pump_window_messages() {
	MSG message;
	while (PeekMessageA(&message, 0, 0, 0, PM_REMOVE)) {
		TranslateMessage(&message);
		DispatchMessageA(&message);
		if (message.message == WM_QUIT) {
			g_context.should_quit = true;
		}
	}
}

static LRESULT CALLBACK on_window_event(
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

		case WM_MOUSEWHEEL: {
			g_context.mouse_events.mouse_wheel_delta += GET_WHEEL_DELTA_WPARAM(w_param) / WHEEL_DELTA;
		} break;

		case WM_MOUSEMOVE: {
			g_context.mouse_events.mouse_x = (int16_t)GET_X_LPARAM(l_param);
			g_context.mouse_events.mouse_y = (int16_t)GET_Y_LPARAM(l_param);
		} break;

		case WM_PAINT: {
			PAINTSTRUCT paint;
			HDC device_context = BeginPaint(window, &paint);
			game::draw(&g_context.window.bitmap, g_context.game);
			engine::render_window(g_context.window, device_context);
			EndPaint(window, &paint);
		} break;
	}

	return DefWindowProc(window, message, w_param, l_param);
}

static engine::AudioID load_audio_from_file(const char* path) {
	HANDLE cowbell_file = CreateFileA(
		path,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		0,
		NULL
	);
	std::expected<engine::AudioID, std::string> result = g_context.audio.add_audio_from_file(cowbell_file);
	CloseHandle(cowbell_file);

	if (!result.has_value()) {
		fprintf(stderr, "Failed to load audio file \"%s\": %s\n", path, result.error().c_str());
		exit(1);
	}

	return result.value();
}

int WINAPI WinMain(
	HINSTANCE instance,
	HINSTANCE /*prev_instance*/,
	LPSTR /*command_line*/,
	int /*command_show*/
) {
	engine::initialize_logging(engine::LogLevel::Debug);
	engine::initialize_gamepad_support();
	g_context.window = initialize_window_or_abort(instance, on_window_event, "Game");
	g_context.audio = engine::initialize_audio_player();
	g_context.assets.audio.cowbell = load_audio_from_file("assets/audio/808_cowbell.wav");

	/* Main loop */
	while (!g_context.should_quit) {
		/* Input */
		pump_window_messages();
		engine::update_input_devices(&g_context.input, g_context.mouse_events);
		g_context.mouse_events = {};

		/* Update */
		game::update(&g_context.game, g_context.input);

		// quick quit while prototyping
		if (g_context.input.keyboard.key_was_pressed_now(VK_ESCAPE)) {
			g_context.should_quit = true;
		}

		// trigger sound with keyboard
		if (g_context.input.keyboard.key_was_pressed_now('1')) {
			g_context.audio.play(g_context.assets.audio.cowbell);
		}

		/* Render */
		auto clear_screen = [bitmap = g_context.window.bitmap]() {
			ZeroMemory(bitmap.data, bitmap.width * bitmap.height * 4);
		};
		auto draw_pixel = [bitmap = g_context.window.bitmap](int x, int y, engine::Color color) {
			struct BGRPixel {
				uint8_t b;
				uint8_t g;
				uint8_t r;
				uint8_t padding;
			};
			if (0 <= x && x < bitmap.width && 0 <= y && y < bitmap.height) {
				((BGRPixel*)bitmap.data)[x + bitmap.width * y] = BGRPixel { color.b, color.g, color.r };
			}
		};
		auto draw_line = [bitmap = g_context.window.bitmap, draw_pixel](engine::IVec2 start, engine::IVec2 end, engine::Color color) {
			// vertical line
			if (start.x == end.x) {
				int y0 = min(start.y, end.y);
				int y1 = max(start.y, end.y);
				for (int32_t y = y0; y <= y1; y++) {
					draw_pixel(start.x, y, color);
				}
			}
			// sloped line
			else {
				int dx = end.x - start.x;
				int dy = end.y - start.y;
				int major_delta = max(std::abs(dx), std::abs(dy));
				float x_step = (float)dx / (float)major_delta;
				float y_step = (float)dy / (float)major_delta;
				for (int32_t i = 0; i <= major_delta; i++) {
					draw_pixel((int)(start.x + i * x_step), (int)(start.y + i * y_step), color);
				}
			}
		};

		clear_screen();

		engine::IVec2 window_center = {
			g_context.window.bitmap.width / 2,
			g_context.window.bitmap.height / 2
		};
		engine::IVec2 start = window_center + engine::IVec2 { 0, 0 };
		engine::IVec2 end = engine::IVec2 { g_context.input.mouse.x, g_context.input.mouse.y };
		engine::Color color = { 0, 255, 0, 255 };
		draw_line(start, end, color);

		HDC device_context = GetDC(g_context.window.handle);
		game::draw(&g_context.window.bitmap, g_context.game);
		engine::render_window(g_context.window, device_context);
		ReleaseDC(g_context.window.handle, device_context);
	}

	return 0;
}
