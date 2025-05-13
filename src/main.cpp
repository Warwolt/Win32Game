#include <core/unwrap.h>
#include <engine/input/gamepad.h>
#include <engine/input/keyboard.h>
#include <engine/logging.h>
#include <engine/render/window.h>

#include <format>
#include <stdio.h>
#include <unordered_map>
#include <unordered_set>
#include <windows.h>

struct GameState {
	// for rendering test gradient
	int x_offset;
	int y_offset;
};

struct ProgramContext {
	// rendering
	engine::Window window;
	// input
	engine::Gamepad gamepad;
	engine::Keyboard keyboard;
	// game
	GameState game;
};

static ProgramContext g_context;

void draw_game(engine::Bitmap* bitmap, const GameState& game) {
	// just draws a gradient right now
	struct Pixel {
		uint8_t b;
		uint8_t g;
		uint8_t r;
		uint8_t padding;
	};
	int row_byte_size = bitmap->width * engine::Bitmap::BYTES_PER_PIXEL;
	uint8_t* current_row = (uint8_t*)bitmap->data;
	for (int y = 0; y < bitmap->height; y++) {
		for (int x = 0; x < bitmap->width; x++) {
			Pixel* pixel = (Pixel*)current_row + x;
			pixel->r = 0;
			pixel->g = (uint8_t)(y + game.y_offset);
			pixel->b = (uint8_t)(x + game.x_offset);
		}
		current_row += row_byte_size;
	}
}

LRESULT CALLBACK on_window_event(
	HWND window,
	UINT message,
	WPARAM w_param,
	LPARAM l_param
) {
	LRESULT result = 0;
	switch (message) {
		case WM_SIZE: {
			if (window == g_context.window.handle) {
				engine::on_window_resized(&g_context.window);
			}
		} break;

		case WM_SYSKEYDOWN:
		case WM_KEYDOWN: {
			uint32_t key_id = (uint32_t)w_param;
			g_context.keyboard.on_key_event(key_id, true);
		} break;

		case WM_SYSKEYUP:
		case WM_KEYUP: {
			uint32_t key_id = (uint32_t)w_param;
			g_context.keyboard.on_key_event(key_id, false);
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
			draw_game(&g_context.window.bitmap, g_context.game);

			PAINTSTRUCT paint;
			HDC device_context = BeginPaint(window, &paint);
			engine::render_window(g_context.window, device_context);
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
	engine::initialize_gamepad();
	g_context.window = core::unwrap(engine::initialize_window(instance, on_window_event), [](engine::WindowError error) {
		std::string message = std::format("Couldn't create window: {}", engine::window_error_to_str(error));
		MessageBoxA(0, message.c_str(), "Error", MB_OK | MB_ICONERROR);
		exit(1);
	});

	/* Main loop */
	bool should_quit = false;
	while (!should_quit) {
		/* Read input */
		{
			/* Message loop */
			MSG message;
			while (PeekMessageA(&message, 0, 0, 0, PM_REMOVE)) {
				TranslateMessage(&message);
				DispatchMessageA(&message);
				if (message.message == WM_QUIT) {
					should_quit = true;
				}
			}

			/* Device input */
			engine::update_gamepad(&g_context.gamepad);
			g_context.keyboard.update();
		}

		/* Update */
		{
			if (g_context.keyboard.key_was_pressed_now(VK_ESCAPE)) {
				should_quit = true;
			}

			constexpr int16_t XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE = 7849;
			if (g_context.gamepad.left_stick_x > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {
				g_context.game.x_offset += g_context.gamepad.left_stick_x / 10000;
			}
			else if (g_context.gamepad.left_stick_x < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {
				g_context.game.x_offset += g_context.gamepad.left_stick_x / 10000;
			}

			if (g_context.gamepad.left_stick_y > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {
				g_context.game.y_offset -= g_context.gamepad.left_stick_y / 10000;
			}
			else if (g_context.gamepad.left_stick_y < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {
				g_context.game.y_offset -= g_context.gamepad.left_stick_y / 10000;
			}
		}

		/* Render */
		{
			draw_game(&g_context.window.bitmap, g_context.game);

			HDC device_context = GetDC(g_context.window.handle);
			engine::render_window(g_context.window, device_context);
			ReleaseDC(g_context.window.handle, device_context);
		}
	}

	return 0;
}
