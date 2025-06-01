#include <engine/graphics/window.h>

namespace engine {

	const char* window_error_to_str(WindowError error) {
		switch (error) {
			case WindowError::FailedToRegisterClass: return "FailedToRegisterClass";
			case WindowError::FailedToCreateWindow: return "FailedToCreateWindow";
		}
		return "";
	}

	// Tries to initialize window, returns nullptr if fails
	std::expected<Window, WindowError> initialize_window(HINSTANCE instance, WNDPROC wnd_proc, const char* window_title) {
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
			return std::unexpected(WindowError::FailedToRegisterClass);
		}

		/* Create window */
		window.handle = CreateWindowExA(
			0,                                // DWORD dwExStyle
			window_class.lpszClassName,       // LPCWSTR lpClassName
			window_title,                     // LPCWSTR lpWindowName
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

		if (!window.handle) {
			return std::unexpected(WindowError::FailedToCreateWindow);
		}

		on_window_resized(&window);
		return window;
	}

	void on_window_resized(Window* window) {
		/* Update size */
		RECT client_rect;
		GetClientRect(window->handle, &client_rect);
		int window_width = client_rect.right - client_rect.left;
		int window_height = client_rect.bottom - client_rect.top;
		window->size = {window_width, window_height};

		/* Re-allocate bitmap */
		if (window->bitmap.data) {
			VirtualFree(window->bitmap.data, 0, MEM_RELEASE);
		}
		int bitmap_size = window_width * window_height * sizeof(BGRPixel);
		window->bitmap.data = (BGRPixel*)VirtualAlloc(0, bitmap_size, MEM_COMMIT, PAGE_READWRITE);
		window->bitmap.width = window_width;
		window->bitmap.height = window_height;
		window->bitmap_info = BITMAPINFO {
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

} // namespace engine
