#include <stdint.h>
#include <stdio.h>
#include <windows.h>
#include <xinput.h>

struct XInputDLL {
	DWORD (*get_state)(DWORD dwUserIndex, XINPUT_STATE* pState);
	DWORD (*set_state)(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration);
};

XInputDLL load_x_input() {
	if (HMODULE x_input_lib = LoadLibraryA("xinput1_4.dll")) {
		return XInputDLL {
			.get_state = (decltype(XInputDLL::get_state))GetProcAddress(x_input_lib, "XInputGetState"),
			.set_state = (decltype(XInputDLL::set_state))GetProcAddress(x_input_lib, "XInputSetState"),
		};
	}
	else {
		fprintf(stderr, "Error, failed to load XInput library!\n");
		return XInputDLL {
			.get_state = [](DWORD dwUserIndex, XINPUT_STATE* pState) -> DWORD { return 0; },
			.set_state = [](DWORD dwUserIndex, XINPUT_VIBRATION* pVibration) -> DWORD { return 0; },
		};
	}
}

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

// Tries to initialize window, returns nullptr if fails
HWND initialize_window(HINSTANCE instance, WNDPROC wnd_proc) {
	/* Register window class */
	WNDCLASSA window_class = {
		.style =
			CS_OWNDC                   // give this window a unique device context
			| CS_HREDRAW | CS_VREDRAW, // redraw entire window when it's resized
		.lpfnWndProc = wnd_proc,
		.hInstance = instance,
		.hCursor = LoadCursor(NULL, IDC_ARROW),
		.lpszClassName = "HandmadeHeroWindowClass",
	};
	if (!RegisterClassA(&window_class)) {
		fprintf(stderr, "failed to register window class, aborting");
		return nullptr;
	}

	/* Create window */
	HWND window_handle = CreateWindowExA(
		0,                                // DWORD dwExStyle
		window_class.lpszClassName,       // LPCWSTR lpClassName
		"Handmade Hero",                  // LPCWSTR lpWindowName
		WS_OVERLAPPEDWINDOW | WS_VISIBLE, // DWORD dwStyle
		CW_USEDEFAULT,                    // int X
		CW_USEDEFAULT,                    // int Y
		CW_USEDEFAULT,                    // int nWidth
		CW_USEDEFAULT,                    // int nHeight
		0,                                // HWND hWndParent
		0,                                // HMENU hMenu
		instance,                         // HINSTANCE hInstance
		0                                 // LPVOID lpParam
	);

	return window_handle;
}

void initialize_printf() {
	/* Get console */
	bool has_console = AttachConsole(ATTACH_PARENT_PROCESS); // attach to parent terminal
	if (!has_console) {
		has_console = AllocConsole(); // create a new console window
	}

	/* Attach std streams */
	if (has_console) {
		FILE* fi = 0;
		freopen_s(&fi, "CONOUT$", "wt", stdout);
		freopen_s(&fi, "CONOUT$", "wt", stderr);
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
	initialize_printf();
	XInputDLL x_input_dll = load_x_input();

	/* Create window */
	HWND window = initialize_window(instance, on_window_event);
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
			for (DWORD controller_index = 0; controller_index < XUSER_MAX_COUNT; controller_index++) {
				XINPUT_STATE controller_state = {};
				if (x_input_dll.get_state(controller_index, &controller_state) == ERROR_SUCCESS) {
					bool dpad_up = controller_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP;
					bool dpad_right = controller_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT;
					bool dpad_down = controller_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN;
					bool dpad_left = controller_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT;
					bool start = controller_state.Gamepad.wButtons & XINPUT_GAMEPAD_START;
					bool back = controller_state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK;
					bool left_shoulder = controller_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER;
					bool right_shoulder = controller_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER;
					bool button_a = controller_state.Gamepad.wButtons & XINPUT_GAMEPAD_A;
					bool button_b = controller_state.Gamepad.wButtons & XINPUT_GAMEPAD_B;
					bool button_x = controller_state.Gamepad.wButtons & XINPUT_GAMEPAD_X;
					bool button_y = controller_state.Gamepad.wButtons & XINPUT_GAMEPAD_Y;
					int16_t stick_x = controller_state.Gamepad.sThumbLX;
					int16_t stick_y = controller_state.Gamepad.sThumbLY;

					if (stick_x > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {
						g_context.game.x_offset += stick_x / 10000;
					}
					else if (stick_x < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {
						g_context.game.x_offset += stick_x / 10000;
					}

					if (stick_y > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {
						g_context.game.y_offset -= stick_y / 10000;
					}
					else if (stick_y < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {
						g_context.game.y_offset -= stick_y / 10000;
					}
				}
				else {
					// controller disconnected
				}
			}
		}

		/* Update */
		{
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
