#pragma once

#include <engine/graphics/bitmap.h>
#include <engine/math/ivec2.h>

#include <windows.h>

#include <expected>
#include <string>

namespace engine {

	enum class WindowError {
		FailedToRegisterClass,
		FailedToCreateWindow,
	};

	class Window {
	public:
		static std::expected<Window, WindowError> initialize(HINSTANCE instance, WNDPROC wnd_proc, IVec2 window_size, const char* window_title);

		IVec2 on_resized();
		void on_focus_changed(bool is_focused);

		IVec2 size() const;
		bool is_focused() const;

		void toggle_fullscreen();
		void set_title(const std::string& title);

		void render(const Bitmap& bitmap);
		void render_wm_paint(const Bitmap& bitmap);

	private:
		void _render(const Bitmap& bitmap, HDC device_context);

		HWND m_handle;
		WINDOWPLACEMENT m_placement = { sizeof(WINDOWPLACEMENT) };
		IVec2 m_window_size;
		bool m_is_focused = true;
	};

	const char* window_error_to_str(WindowError error);

} // namespace engine
