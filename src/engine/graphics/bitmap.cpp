#include <engine/graphics/bitmap.h>

#include <windows.h>

namespace engine {

	Pixel Pixel::from_rgba(RGBA color) {
		return Pixel {
			.b = color.b,
			.g = color.g,
			.r = color.r,
		};
	}

	Pixel Pixel::lerp(Pixel rhs, float t) const {
		return Pixel {
			.b = (uint8_t)std::lerp(this->b, rhs.b, t),
			.g = (uint8_t)std::lerp(this->g, rhs.g, t),
			.r = (uint8_t)std::lerp(this->r, rhs.r, t),
		};
	}

	Bitmap initialize_bitmap(int width, int height) {
		Bitmap bitmap;
		reallocate_bitmap(&bitmap, width, height);
		return bitmap;
	}

	void reallocate_bitmap(Bitmap* bitmap, int width, int height) {
		if (bitmap->data) {
			VirtualFree(bitmap->data, 0, MEM_RELEASE);
		}
		int bitmap_size = width * height * sizeof(Pixel);
		bitmap->data = (Pixel*)VirtualAlloc(0, bitmap_size, MEM_COMMIT, PAGE_READWRITE);
		bitmap->width = width;
		bitmap->height = height;
	}

} // namespace engine
