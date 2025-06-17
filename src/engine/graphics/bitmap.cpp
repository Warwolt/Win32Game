#include <engine/graphics/bitmap.h>

#include <engine/math/math.h>

namespace engine {

	Pixel Pixel::from_rgb(RGBA color) {
		return Pixel {
			.b = color.b,
			.g = color.g,
			.r = color.r,
		};
	}

	Pixel Pixel::lerp(Pixel rhs, float t) const {
		return Pixel {
			.b = (uint8_t)engine::lerp(this->b, rhs.b, t),
			.g = (uint8_t)engine::lerp(this->g, rhs.g, t),
			.r = (uint8_t)engine::lerp(this->r, rhs.r, t),
		};
	}

	Bitmap Bitmap::with_size(int32_t width, int32_t height) {
		Bitmap bitmap;
		bitmap.m_width = std::max(width, 0);
		bitmap.m_height = std::max(height, 0);
		bitmap.m_data.resize(bitmap.m_width * bitmap.m_height);
		return bitmap;
	}

	void Bitmap::clear(Pixel color) {
		for (int32_t y = 0; y < m_height; y++) {
			for (int32_t x = 0; x < m_width; x++) {
				m_data[x + y * m_width] = color;
			}
		}
	}

	void Bitmap::clear_section(Pixel color, Rect rect) {
		for (int32_t y = rect.y; y < rect.height; y++) {
			for (int32_t x = rect.x; x < rect.width; x++) {
				m_data[x + y * m_width] = color;
			}
		}
	}

	void Bitmap::resize(int32_t width, int32_t height) {
		m_width = std::max(width, 0);
		m_height = std::max(height, 0);
		m_data.resize(m_width * m_height);
	}

	void Bitmap::put(int32_t x, int32_t y, Pixel pixel, float alpha) {
		if (0 <= x && x < m_width && 0 <= y && y < m_height) {
			if (alpha == 1.0f) {
				m_data[x + m_width * y] = pixel;
			}
			else {
				m_data[x + m_width * y] = m_data[x + m_width * y].lerp(pixel, alpha);
			}
		}
	}

	Pixel Bitmap::get(int32_t x, int32_t y) {
		if (0 <= x && x < m_width && 0 <= y && y < m_height) {
			return m_data[x + m_width * y];
		}
		return {};
	}

	bool Bitmap::empty() const {
		return m_data.empty();
	}

	int32_t Bitmap::width() const {
		return m_width;
	}

	int32_t Bitmap::height() const {
		return m_height;
	}

	const Pixel* Bitmap::data() const {
		return m_data.data();
	}

} // namespace engine
