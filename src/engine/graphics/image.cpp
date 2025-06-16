#include <engine/graphics/image.h>

#include <engine/math/ivec2.h>

#include <stb_image/stb_image.h>

#include <algorithm>

namespace engine {

	RGBA Image::sample(Vec2 uv) const {
		IVec2 sample_point = IVec2::from_vec2({
			.x = std::clamp(uv.x, 0.0f, 1.0f) * (this->width - 1),
			.y = (1.0f - std::clamp(uv.y, 0.0f, 1.0f)) * (this->height - 1),
		});
		return this->data[sample_point.x + sample_point.y * this->width];
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
		image.data = std::vector<RGBA>(image_data, image_data + length);
		free(image_data);

		return image;
	}

} // namespace engine
