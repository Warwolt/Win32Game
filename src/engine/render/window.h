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

	std::expected<Window, WindowError> initialize_window(HINSTANCE instance, WNDPROC wnd_proc);
	const char* window_error_to_str(WindowError error);
	void on_window_resized(Bitmap* bitmap, HWND window);
	void render_window(const engine::Bitmap& bitmap, HWND window, HDC device_context);

} // namespace engine
