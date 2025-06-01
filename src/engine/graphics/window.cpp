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
			.lpszClassName = "WindowClass",
		};
		if (!RegisterClassA(&window_class)) {
			return std::unexpected(WindowError::FailedToRegisterClass);
		}

		/* Create window */
		window.m_handle = CreateWindowExA(
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

		if (!window.m_handle) {
			return std::unexpected(WindowError::FailedToCreateWindow);
		}

		window.on_resized();
		return window;
	}

	IVec2 Window::on_resized() {
		RECT client_rect;
		GetClientRect(m_handle, &client_rect);
		m_size.x = client_rect.right - client_rect.left;
		m_size.y = client_rect.bottom - client_rect.top;
		return m_size;
	}

	void Window::on_focus_changed(bool is_focused) {
		m_is_focused = is_focused;
	}

	IVec2 Window::size() const {
		return m_size;
	}

	bool Window::is_focused() const {
		return m_is_focused;
	}

	// Based on Raymond Chen's "How do I switch a window between normal and fullscreen?"
	// https://devblogs.microsoft.com/oldnewthing/20100412-00/?p=14353
	void Window::toggle_fullscreen() {
		DWORD style = GetWindowLong(m_handle, GWL_STYLE);
		if (style & WS_OVERLAPPEDWINDOW) {
			MONITORINFO monitor_info = { sizeof(monitor_info) };
			bool got_placement = GetWindowPlacement(m_handle, &m_placement);
			bool got_monitor_info = GetMonitorInfo(MonitorFromWindow(m_handle, MONITOR_DEFAULTTOPRIMARY), &monitor_info);
			if (got_placement && got_monitor_info) {
				SetWindowLong(m_handle, GWL_STYLE, style & ~WS_OVERLAPPEDWINDOW);
				int monitor_width = monitor_info.rcMonitor.right - monitor_info.rcMonitor.left;
				int monitor_height = monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top;
				SetWindowPos(m_handle, HWND_TOP, monitor_info.rcMonitor.left, monitor_info.rcMonitor.top, monitor_width, monitor_height, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
			}
		}
		else {
			SetWindowLong(m_handle, GWL_STYLE, style | WS_OVERLAPPEDWINDOW);
			SetWindowPlacement(m_handle, &m_placement);
			SetWindowPos(m_handle, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
		}
	}

	void Window::render(const Bitmap& bitmap) {
		HDC device_context = GetDC(m_handle);
		_render(bitmap, device_context);
		ReleaseDC(m_handle, device_context);
	}

	void Window::render_wm_paint(const Bitmap& bitmap) {
		PAINTSTRUCT paint;
		HDC device_context = BeginPaint(m_handle, &paint);
		_render(bitmap, device_context);
		EndPaint(m_handle, &paint);
	}

	void Window::_render(const Bitmap& bitmap, HDC device_context) {
		RECT client_rect;
		GetClientRect(m_handle, &client_rect);
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
