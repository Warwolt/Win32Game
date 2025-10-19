#include <engine/graphics/window.h>

// undo windows.h macros
#undef min
#undef max

#include <engine/debug/profiling.h>
#include <engine/graphics/rect.h>

#include <engine/debug/logging.h>

#include <algorithm>

namespace engine {

	std::optional<Window> Window::initialize(HINSTANCE instance, WNDPROC wnd_proc, IVec2 window_size, const char* window_title) {
		Window window = {};
		window.m_window_size = window_size;

		/* Register window class */
		WNDCLASSA window_class = {
			.style =
				CS_OWNDC                   // give this window a unique device context
				| CS_HREDRAW | CS_VREDRAW, // redraw entire window when it's resized
			.lpfnWndProc = wnd_proc,
			.hInstance = instance,
			.hCursor = LoadCursor(NULL, IDC_ARROW),
			.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH), // set black background
			.lpszClassName = "WindowClass",
		};
		if (!RegisterClassA(&window_class)) {
			LOG_ERROR("Failed to register window class. RegisterClassA gave error: %d", GetLastError());
			return {};
		}

		/* Compute window size */
		// NOTE: CreateWindowExA width and height will include the menu bar and
		// border, so we need to compute what the window size should be from
		// the given client size (the `window_size` parameter).
		RECT adjusted_window_size = { 0, 0, window_size.x, window_size.y };
		AdjustWindowRect(&adjusted_window_size, WS_OVERLAPPEDWINDOW, FALSE);
		int adjusted_window_width = adjusted_window_size.right - adjusted_window_size.left;
		int adjusted_window_height = adjusted_window_size.bottom - adjusted_window_size.top;

		/* Get monitor center */
		RECT desktop_rect;
		GetWindowRect(GetDesktopWindow(), &desktop_rect);
		int32_t monitor_center_x = desktop_rect.right / 2;
		int32_t monitor_center_y = desktop_rect.bottom / 2;

		/* Create window */
		window.m_handle = CreateWindowExA(
			0,                                                                   // DWORD dwExStyle
			window_class.lpszClassName,                                          // LPCWSTR lpClassName
			window_title,                                                        // LPCWSTR lpWindowName
			WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME | WS_VISIBLE, // DWORD dwStyle
			monitor_center_x - window.m_window_size.x / 2,                       // int X
			monitor_center_y - window.m_window_size.y / 2,                       // int Y
			adjusted_window_width,                                               // int nWidth
			adjusted_window_height,                                              // int nHeight
			0,                                                                   // HWND hWndParent
			0,                                                                   // HMENU hMenu
			instance,                                                            // HINSTANCE hInstance
			0                                                                    // LPVOID lpParam
		);

		if (!window.m_handle) {
			LOG_ERROR("Failed to create window. CreateWindowExA gave error: %d", GetLastError());
			return {};
		}

		return window;
	}

	IVec2 Window::on_resized() {
		RECT client_rect;
		GetClientRect(m_handle, &client_rect);
		m_window_size.x = client_rect.right - client_rect.left;
		m_window_size.y = client_rect.bottom - client_rect.top;
		return m_window_size;
	}

	void Window::on_focus_changed(bool is_focused) {
		m_is_focused = is_focused;
	}

	IVec2 Window::size() const {
		return m_window_size;
	}

	bool Window::is_focused() const {
		return m_is_focused;
	}

	bool Window::is_minimized() const {
		return IsIconic(m_handle);
	}

	// Based on Raymond Chen's "How do I switch a window between normal and fullscreen?"
	// https://devblogs.microsoft.com/oldnewthing/20100412-00/?p=14353
	void Window::toggle_fullscreen() {
		DWORD style = GetWindowLong(m_handle, GWL_STYLE);
		if (style & WS_OVERLAPPEDWINDOW) {
			GetWindowPlacement(m_handle, &m_placement);
			SetWindowLong(m_handle, GWL_STYLE, style & ~WS_OVERLAPPEDWINDOW);
			MONITORINFO monitor_info = { .cbSize = sizeof(monitor_info) };
			GetMonitorInfo(MonitorFromWindow(m_handle, MONITOR_DEFAULTTOPRIMARY), &monitor_info);
			int32_t monitor_x = monitor_info.rcMonitor.left;
			int32_t monitor_y = monitor_info.rcMonitor.top;
			int32_t monitor_width = monitor_info.rcMonitor.right - monitor_info.rcMonitor.left;
			int32_t monitor_height = monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top;
			SetWindowPos(m_handle, HWND_TOP, monitor_x, monitor_y, monitor_width, monitor_height, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
		}
		else {
			SetWindowLong(m_handle, GWL_STYLE, style | WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME);
			SetWindowPlacement(m_handle, &m_placement);
			SetWindowPos(m_handle, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
		}
	}

	void Window::set_title(const std::string& title) {
		SetWindowTextA(m_handle, title.c_str());
	}

	void Window::render(const Bitmap& bitmap) {
		CPUProfilingScope_Render();
		HDC device_context = GetDC(m_handle);
		_render(bitmap, device_context);
		ReleaseDC(m_handle, device_context);
	}

	void Window::render_wm_paint(const Bitmap& bitmap) {
		CPUProfilingScope_Render();
		PAINTSTRUCT paint;
		HDC device_context = BeginPaint(m_handle, &paint);
		_render(bitmap, device_context);
		EndPaint(m_handle, &paint);
	}

	void Window::_render(const Bitmap& bitmap, HDC device_context) {
		// Can't render empty bitmap
		if (bitmap.empty()) {
			return;
		}

		BITMAPINFO bitmap_info = BITMAPINFO {
			.bmiHeader = BITMAPINFOHEADER {
				.biSize = sizeof(BITMAPINFOHEADER),
				.biWidth = bitmap.width(),
				.biHeight = -bitmap.height(),
				.biPlanes = 1,
				.biBitCount = 32,
				.biCompression = BI_RGB,
			}
		};

		int scale = std::min(
			std::max(m_window_size.x / bitmap.width(), 1),
			std::max(m_window_size.y / bitmap.height(), 1)
		);
		IVec2 upscaled_bitmap_size = scale * IVec2 { bitmap.width(), bitmap.height() };

		StretchDIBits(
			device_context,

			// destination rect (window)
			(m_window_size.x - upscaled_bitmap_size.x) / 2,
			(m_window_size.y - upscaled_bitmap_size.y) / 2,
			upscaled_bitmap_size.x,
			upscaled_bitmap_size.y,

			// source rect (bitmap)
			0,
			0,
			bitmap.width(),
			bitmap.height(),

			// bitmap data
			bitmap.data(),
			&bitmap_info,
			DIB_RGB_COLORS,
			SRCCOPY
		);
	}

} // namespace engine
