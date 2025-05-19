#include <engine/graphics/bitmap.h>

#include <expected>
#include <windows.h>

namespace engine {

	struct Window {
		HWND handle;
		Bitmap bitmap;
		BITMAPINFO bitmap_info;
	};

	enum class WindowError {
		FailedToRegisterClass,
		FailedToCreateWindow,
	};
	const char* window_error_to_str(WindowError error);

	std::expected<Window, WindowError> initialize_window(HINSTANCE instance, WNDPROC wnd_proc, const char* window_title);
	void on_window_resized(Window* window);
	void render(const Window& window, HDC device_context);

} // namespace engine
