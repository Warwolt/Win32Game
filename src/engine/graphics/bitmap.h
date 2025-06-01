#pragma once

#include <engine/graphics/color.h>

#include <stdint.h>

namespace engine {

	struct BGRPixel {
		uint8_t b;
		uint8_t g;
		uint8_t r;
		uint8_t padding;

		inline static BGRPixel from_rgba(Color color) {
			return {
				.b = color.b,
				.g = color.g,
				.r = color.r,
			};
		}
	};

	struct Bitmap {
		BGRPixel* data;
		int32_t width;
		int32_t height;

		inline void put(int32_t x, int32_t y, BGRPixel pixel) {
			if (0 <= x && x < this->width && 0 <= y && y < this->height) {
				this->data[x + this->width * y] = pixel;
			}
		}

		inline BGRPixel get(int32_t x, int32_t y) {
			if (0 <= x && x < this->width && 0 <= y && y < this->height) {
				return this->data[x + this->width * y];
			}
			return {};
		}
	};

	Bitmap initialize_bitmap(int width, int height);
	void reallocate_bitmap(Bitmap* bitmap, int width, int height);

} // namespace engine
