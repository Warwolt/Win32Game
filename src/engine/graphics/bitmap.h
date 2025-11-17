#pragma once

#include <engine/graphics/image.h>
#include <engine/graphics/rect.h>
#include <engine/graphics/rgba.h>

#include <cmath>
#include <stdint.h>
#include <vector>

namespace engine {

	// Win32 endian stuff means RGB is stored as BGR.
	// For alignment reasons we also add `padding`.
	struct Pixel {
		uint8_t b;
		uint8_t g;
		uint8_t r;
		uint8_t padding;

		static Pixel from_rgb(RGBA color);
		bool operator==(const Pixel& rhs) const = default;
		Pixel lerp(Pixel rhs, float t) const;
	};

	class Bitmap {
	public:
		Bitmap() = default;
		static Bitmap with_size(int32_t width, int32_t height);
		Image to_image() const;

		void clear(Pixel color);
		void resize(int32_t width, int32_t height);
		void put(int32_t x, int32_t y, Pixel pixel, float alpha);
		Pixel get(int32_t x, int32_t y);
		bool empty() const;
		int32_t width() const;
		int32_t height() const;
		IVec2 size() const;
		const Pixel* data() const;
		bool operator==(const Bitmap& rhs) const = default;

	private:
		int32_t m_width = 0;
		int32_t m_height = 0;
		std::vector<Pixel> m_data;
	};

} // namespace engine
