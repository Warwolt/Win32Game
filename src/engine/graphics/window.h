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
		IVec2 on_resized();
		void toggle_fullscreen();
		void render(const Bitmap& bitmap, HDC device_context);

		// FIXME: move these into private
		HWND handle;
		WINDOWPLACEMENT placement = { sizeof(WINDOWPLACEMENT) };
		IVec2 size;
		bool is_focused = true;

	private:
	};

	const char* window_error_to_str(WindowError error);
	// void toggle_window_fullscreen(Window* window);

} // namespace engine
