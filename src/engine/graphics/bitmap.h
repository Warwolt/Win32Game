#pragma once

#include <engine/graphics/rgba.h>

#include <cmath>
#include <stdint.h>


namespace engine {

	struct Pixel {
		uint8_t b;
		uint8_t g;
		uint8_t r;
		uint8_t padding;

		static Pixel from_rgba(RGBA color);
		bool operator==(const Pixel& rhs) const = default;
		Pixel lerp(Pixel rhs, float t) const;
	};

	struct Bitmap {
		Pixel* data;
		int32_t width;
		int32_t height;

		inline void put(int32_t x, int32_t y, Pixel pixel) {
			if (0 <= x && x < this->width && 0 <= y && y < this->height) {
				this->data[x + this->width * y] = pixel;
			}
		}

		inline Pixel get(int32_t x, int32_t y) {
			if (0 <= x && x < this->width && 0 <= y && y < this->height) {
				return this->data[x + this->width * y];
			}
			return {};
		}
	};

	Bitmap initialize_bitmap(int width, int height);
	void reallocate_bitmap(Bitmap* bitmap, int width, int height);

} // namespace engine
