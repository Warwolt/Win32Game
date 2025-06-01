#pragma once

#include <engine/graphics/bitmap.h>
#include <engine/math/ivec2.h>

#include <expected>
#include <windows.h>

namespace engine {

	enum class WindowError {
		FailedToRegisterClass,
		FailedToCreateWindow,
	};

	class Window {
	public:
		static std::expected<Window, WindowError> initialize(HINSTANCE instance, WNDPROC wnd_proc, const char* window_title);

		HWND m_handle;

		IVec2 size() const;
		bool is_focused() const;

		IVec2 on_resized();
		void on_focus_changed(bool is_focused);

		void toggle_fullscreen();
		void render(const Bitmap& bitmap, HDC device_context);

	private:
		WINDOWPLACEMENT m_placement = { sizeof(WINDOWPLACEMENT) };
		IVec2 m_size;
		bool m_is_focused = true;
	};

	const char* window_error_to_str(WindowError error);

} // namespace engine
