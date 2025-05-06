#include <optional>
#include <stdio.h>
#include <windows.h>

struct RenderContext {
	BITMAPINFO bitmap_info;
	void* bitmap_data;
	HBITMAP bitmap_handle;
	int bitmap_width;
	int bitmap_height;
};
RenderContext g_render_context;

namespace core {
	template <typename T, typename F>
	T unwrap(std::optional<T>&& optional, F&& on_error) {
		if (optional) {
			return *optional;
		}
		on_error();
		throw std::bad_optional_access();
	}
} // namespace core

void resize_dib_section(RenderContext* render_context, int width, int height) {
	if (render_context->bitmap_data) {
		VirtualFree(render_context->bitmap_data, 0, MEM_RELEASE);
	}

	constexpr int BYTES_PER_PIXEL = 4;
	int bitmap_size = width * height * BYTES_PER_PIXEL;
	render_context->bitmap_data = VirtualAlloc(0, bitmap_size, MEM_COMMIT, PAGE_READWRITE);
	render_context->bitmap_width = width;
	render_context->bitmap_height = height;
	render_context->bitmap_info = {
		.bmiHeader = {
			.biSize = sizeof(BITMAPINFOHEADER),
			.biWidth = width,
			.biHeight = -height,
			.biPlanes = 1,
			.biBitCount = 32,
			.biCompression = BI_RGB,
		}
	};

	// hack some data into the bitmap
	struct Pixel {
		uint8_t b;
		uint8_t g;
		uint8_t r;
		uint8_t padding;
	};
	int row_byte_size = width * BYTES_PER_PIXEL;
	uint8_t* current_row = (uint8_t*)render_context->bitmap_data;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			Pixel* pixel = (Pixel*)current_row + x;
			pixel->r = 0;
			pixel->g = (uint8_t)y;
			pixel->b = (uint8_t)x;
		}
		current_row += row_byte_size;
	}
}

void paint_window(const RenderContext& render_context, HDC device_context, const RECT& window_rect, int x, int y, int width, int height) {
	int window_width = window_rect.right - window_rect.left;
	int window_height = window_rect.bottom - window_rect.top;
	StretchDIBits(
		device_context,
		// destination rect
		0,
		0,
		render_context.bitmap_width,
		render_context.bitmap_height,
		// source rect
		0,
		0,
		window_width,
		window_height,
		// data
		render_context.bitmap_data,
		&render_context.bitmap_info,
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
			RECT client_rect;
			GetClientRect(window, &client_rect);
			int width = client_rect.right - client_rect.left;
			int height = client_rect.bottom - client_rect.top;
			resize_dib_section(&g_render_context, width, height);
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
				paint_window(g_render_context, device_context, client_rect, x, y, width, height);
			}
			EndPaint(window, &paint);
		} break;
	}

	return DefWindowProc(window, message, w_param, l_param);
}

std::optional<HWND> initialize_window(HINSTANCE instance) {
	/* Register window class */
	WNDCLASSA window_class = {
		.style =
			CS_OWNDC                   // give this window a unique device context
			| CS_HREDRAW | CS_VREDRAW, // redraw window when resized
		.lpfnWndProc = on_window_event,
		.hInstance = instance,
		.hCursor = LoadCursor(NULL, IDC_ARROW),
		.lpszClassName = "HandmadeHeroWindowClass",
	};
	if (!RegisterClassA(&window_class)) {
		fprintf(stderr, "failed to register window class, aborting");
		return std::nullopt;
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
	if (!window_handle) {
		// TODO handle and abort
		return std::nullopt;
	}

	return window_handle;
}

void initialize_printf() {
	// get a console
	bool has_console = AttachConsole(ATTACH_PARENT_PROCESS);
	if (!has_console) {
		has_console = AllocConsole();
	}
	// attach std streams
	if (has_console) {
		FILE* fi = 0;
		freopen_s(&fi, "CONOUT$", "wt", stdout);
		freopen_s(&fi, "CONOUT$", "wt", stderr);
	}
}

int WINAPI WinMain(
	HINSTANCE instance,
	HINSTANCE /*prev_instance*/,
	LPSTR /*command_line*/,
	int /*command_show*/
) {
	initialize_printf();
	printf("Program Start\n");

	/* Create window */
	HWND window_handle = core::unwrap(initialize_window(instance), [] {
		fprintf(stderr, "Couldn't initialize window");
	});

	/* Handle messages */
	while (true) {
		MSG message;
		if (GetMessageW(&message, 0, 0, 0) <= 0) {
			break;
		}

		TranslateMessage(&message);
		DispatchMessageW(&message);
	}

	return 0;
}
