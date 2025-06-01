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

	void Window::on_resized() {
		/* Update size */
		RECT client_rect;
		GetClientRect(this->handle, &client_rect);
		int window_width = client_rect.right - client_rect.left;
		int window_height = client_rect.bottom - client_rect.top;
		this->size = { window_width, window_height };

		/* Re-allocate bitmap */
		if (this->bitmap.data) {
			VirtualFree(this->bitmap.data, 0, MEM_RELEASE);
		}
		int bitmap_size = window_width * window_height * sizeof(BGRPixel);
		this->bitmap.data = (BGRPixel*)VirtualAlloc(0, bitmap_size, MEM_COMMIT, PAGE_READWRITE);
		this->bitmap.width = window_width;
		this->bitmap.height = window_height;
	}

} // namespace engine
