#include <engine/graphics/bitmap.h>

#include <windows.h>

namespace engine {

	Bitmap initialize_bitmap(int width, int height) {
		Bitmap bitmap;
		reallocate_bitmap(&bitmap, width, height);
		return bitmap;
	}

	void reallocate_bitmap(Bitmap* bitmap, int width, int height) {
		if (bitmap->data) {
			VirtualFree(bitmap->data, 0, MEM_RELEASE);
		}
		int bitmap_size = width * height * sizeof(BGRPixel);
		bitmap->data = (BGRPixel*)VirtualAlloc(0, bitmap_size, MEM_COMMIT, PAGE_READWRITE);
		bitmap->width = width;
		bitmap->height = height;
	}

} // namespace engine
