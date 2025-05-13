#include <engine/input/gamepad.h>
#include <engine/render/window.h>
#include <engine/logging.h>

#include <stdio.h>
#include <windows.h>

struct Bitmap {
	BITMAPINFO info;
	void* data;
	HBITMAP handle;
	int width;
	int height;
	static constexpr int BYTES_PER_PIXEL = 4;
};

struct GameState {
	// for rendering test gradient
	int x_offset;
	int y_offset;
};

struct ProgramContext {
	Bitmap bitmap;
	engine::Gamepad gamepad;
	GameState game;
};

static ProgramContext g_context;

void draw_game(Bitmap* bitmap, const GameState& game) {
	// just draws a gradient right now
	struct Pixel {
		uint8_t b;
		uint8_t g;
		uint8_t r;
		uint8_t padding;
	};
	int row_byte_size = bitmap->width * Bitmap::BYTES_PER_PIXEL;
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

void resize_window_bitmap(Bitmap* bitmap, HWND window) {
	RECT client_rect;
	GetClientRect(window, &client_rect);
	int window_width = client_rect.right - client_rect.left;
	int window_height = client_rect.bottom - client_rect.top;

	if (bitmap->data) {
		VirtualFree(bitmap->data, 0, MEM_RELEASE);
	}

	int bitmap_size = window_width * window_height * Bitmap::BYTES_PER_PIXEL;
	bitmap->data = VirtualAlloc(0, bitmap_size, MEM_COMMIT, PAGE_READWRITE);
	bitmap->width = window_width;
	bitmap->height = window_height;
	bitmap->info = BITMAPINFO {
		.bmiHeader = BITMAPINFOHEADER {
			.biSize = sizeof(BITMAPINFOHEADER),
			.biWidth = window_width,
			.biHeight = -window_height,
			.biPlanes = 1,
			.biBitCount = 32,
			.biCompression = BI_RGB,
		}
	};
}

void render_window(const Bitmap& bitmap, HWND window, HDC device_context) {
	RECT client_rect;
	GetClientRect(window, &client_rect);
	int window_width = client_rect.right - client_rect.left;
	int window_height = client_rect.bottom - client_rect.top;

	StretchDIBits(
		device_context,
		// destination rect (window)
		0,
		0,
		window_width,
		window_height,
		// source rect (bitmap)
		0,
		0,
		bitmap.width,
		bitmap.height,
		// bitmap data
		bitmap.data,
		&bitmap.info,
		DIB_RGB_COLORS,
		SRCCOPY
	);
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
			resize_window_bitmap(&g_context.bitmap, window);
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
				draw_game(&g_context.bitmap, g_context.game);
				render_window(g_context.bitmap, window, device_context);
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
	engine::initialize_gamepad();

	/* Create window */
	HWND window = engine::initialize_window(instance, on_window_event);
	if (!window) {
		fprintf(stderr, "Couldn't initialize window, aborting");
		return 1;
	};

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
		}

		/* Update */
		{
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
			HDC device_context = GetDC(window);
			{
				draw_game(&g_context.bitmap, g_context.game);
				render_window(g_context.bitmap, window, device_context);
			}
			ReleaseDC(window, device_context);
		}
	}

	return 0;
}
