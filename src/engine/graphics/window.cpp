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
	std::expected<Window, WindowError> Window::initialize(HINSTANCE instance, WNDPROC wnd_proc, const char* window_title) {
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

		window.on_resized();
		return window;
	}

	IVec2 Window::on_resized() {
		RECT client_rect;
		GetClientRect(this->handle, &client_rect);
		this->size.x = client_rect.right - client_rect.left;
		this->size.y = client_rect.bottom - client_rect.top;
		return this->size;
	}

	void Window::render(const Bitmap& bitmap, HDC device_context) {
		RECT client_rect;
		GetClientRect(this->handle, &client_rect);
		int window_width = client_rect.right - client_rect.left;
		int window_height = client_rect.bottom - client_rect.top;

		BITMAPINFO bitmap_info = BITMAPINFO {
			.bmiHeader = BITMAPINFOHEADER {
				.biSize = sizeof(BITMAPINFOHEADER),
				.biWidth = window_width,
				.biHeight = -window_height,
				.biPlanes = 1,
				.biBitCount = 32,
				.biCompression = BI_RGB,
			}
		};

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
			&bitmap_info,
			DIB_RGB_COLORS,
			SRCCOPY
		);
	}

} // namespace engine
