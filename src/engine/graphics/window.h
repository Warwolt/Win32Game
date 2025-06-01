#pragma once

#include <engine/graphics/bitmap.h>
#include <engine/math/ivec2.h>

#include <expected>
#include <windows.h>

namespace engine {

	// FIXME: make this a class?
	struct Window {
		HWND handle;
		WINDOWPLACEMENT placement = { sizeof(WINDOWPLACEMENT) };
		Bitmap bitmap;
		IVec2 size;
		bool is_focused = true;
	};

	enum class WindowError {
		FailedToRegisterClass,
		FailedToCreateWindow,
	};
	const char* window_error_to_str(WindowError error);

	std::expected<Window, WindowError> initialize_window(HINSTANCE instance, WNDPROC wnd_proc, const char* window_title);
	void on_window_resized(Window* window);
	// void toggle_window_fullscreen(Window* window);

} // namespace engine
