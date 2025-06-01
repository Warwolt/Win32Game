#pragma once

#include <engine/graphics/bitmap.h>
#include <engine/math/ivec2.h>

#include <expected>
#include <windows.h>

namespace engine {

	struct Window {
		HWND handle;
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

} // namespace engine
