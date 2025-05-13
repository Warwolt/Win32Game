#include <expected>

#include <windows.h>

namespace engine {

	struct Bitmap {
		BITMAPINFO info;
		void* data;
		HBITMAP handle;
		int width;
		int height;
		static constexpr int BYTES_PER_PIXEL = 4;
	};

	struct Window {
		HWND handle;
		Bitmap bitmap;
	};

	enum class WindowError {
		FailedToRegisterClass,
		FailedToCreateWindow,
	};
	const char* window_error_to_str(WindowError error);

	std::expected<Window, WindowError> initialize_window(HINSTANCE instance, WNDPROC wnd_proc);
	void on_window_resized(Window* window);
	void render_window(const Window& window, HDC device_context);

} // namespace engine
