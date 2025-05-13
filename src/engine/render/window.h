#include <windows.h>

namespace engine {

	struct WindowBitmap {
		BITMAPINFO info;
		void* data;
		HBITMAP handle;
		int width;
		int height;
		static constexpr int BYTES_PER_PIXEL = 4;
	};

	HWND initialize_window(HINSTANCE instance, WNDPROC wnd_proc);
	void on_window_resized(WindowBitmap* bitmap, HWND window);
	void render_window(const engine::WindowBitmap& bitmap, HWND window, HDC device_context);

} // namespace engine
