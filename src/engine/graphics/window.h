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
		void on_resized();

		HWND handle;
		WINDOWPLACEMENT placement = { sizeof(WINDOWPLACEMENT) };
		Bitmap bitmap;
		IVec2 size;
		bool is_focused = true;

	private:
	};

	const char* window_error_to_str(WindowError error);
	// void toggle_window_fullscreen(Window* window);

} // namespace engine
