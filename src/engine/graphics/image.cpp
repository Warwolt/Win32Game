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

	std::optional<Image> load_image(const char* image_path) {
		Image image;
		int num_channels = 0;
		constexpr int num_requested_channels = 4; // RGBA
		image.data = (RGBA*)stbi_load(image_path, &image.width, &image.height, &num_channels, num_requested_channels);
		if (!image.data) {
			return {};
		}
		return image;
	}

	void free_image(const Image& image) {
		stbi_image_free(image.data);
	}

} // namespace engine
