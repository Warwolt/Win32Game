#include <optional>
#include <stdio.h>
#include <windows.h>

struct GameState {
	// for rendering test gradient
	int x_offset;
};

struct Bitmap {
	BITMAPINFO info;
	void* data;
	HBITMAP handle;
	int width;
	int height;
	static constexpr int BYTES_PER_PIXEL = 4;
};

struct ProgramContext {
	Bitmap bitmap;
	GameState game;
};

static ProgramContext g_context;

void draw_gradient(const Bitmap& bitmap, int width, int height, int x_offset) {
	struct Pixel {
		uint8_t b;
		uint8_t g;
		uint8_t r;
		uint8_t padding;
	};
	int row_byte_size = width * Bitmap::BYTES_PER_PIXEL;
	uint8_t* current_row = (uint8_t*)bitmap.data;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			Pixel* pixel = (Pixel*)current_row + x;
			pixel->r = 0;
			pixel->g = (uint8_t)y;
			pixel->b = (uint8_t)(x + x_offset);
		}
		current_row += row_byte_size;
	}
}

void resize_bitmap(Bitmap* bitmap, int width, int height) {
	if (bitmap->data) {
		VirtualFree(bitmap->data, 0, MEM_RELEASE);
	}

	int bitmap_size = width * height * Bitmap::BYTES_PER_PIXEL;
	bitmap->data = VirtualAlloc(0, bitmap_size, MEM_COMMIT, PAGE_READWRITE);
	bitmap->width = width;
	bitmap->height = height;
	bitmap->info = {
		.bmiHeader = {
			.biSize = sizeof(BITMAPINFOHEADER),
			.biWidth = width,
			.biHeight = -height,
			.biPlanes = 1,
			.biBitCount = 32,
			.biCompression = BI_RGB,
		}
	};
}

void paint_bitmap_onto_window(const Bitmap& bitmap, HWND window, HDC device_context) {
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
			RECT client_rect;
			GetClientRect(window, &client_rect);
			int width = client_rect.right - client_rect.left;
			int height = client_rect.bottom - client_rect.top;
			resize_bitmap(&g_context.bitmap, width, height);
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
				draw_gradient(g_context.bitmap, g_context.bitmap.width, g_context.bitmap.height, g_context.game.x_offset);
				paint_bitmap_onto_window(g_context.bitmap, window, device_context);
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

	/* Create window */
	HWND window = initialize_window(instance, on_window_event);
	if (!window) {
		fprintf(stderr, "Couldn't initialize window, aborting");
		return 1;
	};

	/* Main loop */
	bool should_quit = false;
	while (!should_quit) {
		/* Process messages */
		{
			MSG message;
			while (PeekMessageA(&message, 0, 0, 0, PM_REMOVE)) {
				TranslateMessage(&message);
				DispatchMessageA(&message);
				if (message.message == WM_QUIT) {
					should_quit = true;
				}
			}
		}

		/* Update */
		{
			g_context.game.x_offset += 1;
		}

		/* Render */
		{
			// draw game
			draw_gradient(g_context.bitmap, g_context.bitmap.width, g_context.bitmap.height, g_context.game.x_offset);

			// render
			HDC device_context = GetDC(window);
			{
				RECT client_rect;
				GetClientRect(window, &client_rect);
				int window_width = client_rect.right - client_rect.left;
				int window_height = client_rect.bottom - client_rect.top;
				paint_bitmap_onto_window(g_context.bitmap, window, device_context);
			}
			ReleaseDC(window, device_context);
		}
	}

	return 0;
}
