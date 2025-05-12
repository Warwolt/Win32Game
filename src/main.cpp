#include <optional>
#include <stdio.h>
#include <windows.h>

struct GameState {
	// for rendering test gradient
	int x_offset;
};

struct RenderContext {
	BITMAPINFO bitmap_info;
	void* bitmap_data;
	HBITMAP bitmap_handle;
	int bitmap_width;
	int bitmap_height;
};

struct ProgramContext {
	RenderContext rendering;
	GameState game;
};

static ProgramContext g_context;

constexpr int BYTES_PER_BITMAP_PIXEL = 4;

void draw_gradient(const RenderContext& rendering, int width, int height, int x_offset) {
	struct Pixel {
		uint8_t b;
		uint8_t g;
		uint8_t r;
		uint8_t padding;
	};
	int row_byte_size = width * BYTES_PER_BITMAP_PIXEL;
	uint8_t* current_row = (uint8_t*)rendering.bitmap_data;
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

void resize_window_bitmap(RenderContext* rendering, int width, int height) {
	if (rendering->bitmap_data) {
		VirtualFree(rendering->bitmap_data, 0, MEM_RELEASE);
	}

	int bitmap_size = width * height * BYTES_PER_BITMAP_PIXEL;
	rendering->bitmap_data = VirtualAlloc(0, bitmap_size, MEM_COMMIT, PAGE_READWRITE);
	rendering->bitmap_width = width;
	rendering->bitmap_height = height;
	rendering->bitmap_info = {
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

void paint_window_bitmap(const RenderContext& rendering, HDC device_context, const RECT& window_rect, int x, int y, int width, int height) {
	int window_width = window_rect.right - window_rect.left;
	int window_height = window_rect.bottom - window_rect.top;
	StretchDIBits(
		device_context,
		// source rect (bitmap)
		0,
		0,
		rendering.bitmap_width,
		rendering.bitmap_height,
		// destination rect (window)
		0,
		0,
		window_width,
		window_height,
		// bitmap data
		rendering.bitmap_data,
		&rendering.bitmap_info,
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
			| CS_HREDRAW | CS_VREDRAW, // redraw window when resized
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
			resize_window_bitmap(&g_context.rendering, width, height);
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
				RECT client_rect;
				GetClientRect(window, &client_rect);
				int x = paint.rcPaint.left;
				int y = paint.rcPaint.top;
				int width = paint.rcPaint.right - paint.rcPaint.left;
				int height = paint.rcPaint.top - paint.rcPaint.bottom;
				draw_gradient(g_context.rendering, g_context.rendering.bitmap_width, g_context.rendering.bitmap_height, g_context.game.x_offset);
				paint_window_bitmap(g_context.rendering, device_context, client_rect, x, y, width, height);
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
			draw_gradient(g_context.rendering, g_context.rendering.bitmap_width, g_context.rendering.bitmap_height, g_context.game.x_offset);

			// render
			HDC device_context = GetDC(window);
			{
				RECT client_rect;
				GetClientRect(window, &client_rect);
				int window_width = client_rect.right - client_rect.left;
				int window_height = client_rect.bottom - client_rect.top;
				paint_window_bitmap(g_context.rendering, device_context, client_rect, 0, 0, window_width, window_height);
			}
			ReleaseDC(window, device_context);
		}
	}

	return 0;
}
