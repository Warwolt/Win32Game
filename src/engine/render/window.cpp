#include <engine/render/window.h>

#include <stdio.h>

namespace engine {

	// Tries to initialize window, returns nullptr if fails
	Window initialize_window(HINSTANCE instance, WNDPROC wnd_proc) {
		Window window;

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
			return {};
		}

		/* Create window */
		window.handle = CreateWindowExA(
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

		if (window.handle) {
			on_window_resized(&window.bitmap, window.handle);
		}

		return window;
	}

	void on_window_resized(Bitmap* bitmap, HWND window) {
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

} // namespace engine
