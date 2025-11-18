#include <engine/graphics/image.h>

#include <engine/math/math.h>

#include <cmath>

#include <stb_image/stb_image.h>

namespace engine {

	RGBA Image::sample(Vec2 uv) const {
		int32_t sample_point_x = (int32_t)std::round(uv.x * (this->width - 1));
		int32_t sample_point_y = (int32_t)std::round((1.0f - uv.y) * (this->height - 1));
		return this->pixels[sample_point_x + sample_point_y * this->width];
	}

	RGBA Image::get(int x, int y) const {
		int32_t clamped_x = engine::clamp(x, 0, this->width - 1);
		int32_t clamped_y = engine::clamp(y, 0, this->height - 1);
		return this->pixels[clamped_x + clamped_y * this->width];
	}

	std::optional<Image> Image::from_path(std::filesystem::path path) {
		Image image;

		/* Load image using STBI */
		int num_channels = 0;
		constexpr int num_requested_channels = 4; // RGBA
		RGBA* image_data = (RGBA*)stbi_load((const char*)path.string().c_str(), &image.width, &image.height, &num_channels, num_requested_channels);
		if (!image_data) {
			return {};
		}
		size_t length = image.width * image.height;

		/* Copy STBI data to our own vector */
		image.pixels = std::vector<RGBA>(image_data, image_data + length);
		free(image_data);

		return image;
	}

} // namespace engine
