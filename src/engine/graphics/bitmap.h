#include <stdint.h>

namespace engine {

	struct BGRPixel {
		uint8_t b;
		uint8_t g;
		uint8_t r;
		uint8_t padding;
	};

	struct Bitmap {
		BGRPixel* data;
		int width;
		int height;

		inline void put(int32_t x, int32_t y, BGRPixel pixel) {
			if (0 <= x && x < this->width && 0 <= y && y < this->height) {
				this->data[x + this->width * y] = pixel;
			}
		}
	};

} // namespace engine
