#pragma once

#include <engine/graphics/rgba.h>
#include <engine/math/vec2.h>

#include <optional>
#include <vector>

namespace engine {

	struct Image {
		int width = 0;
		int height = 0;
		std::vector<RGBA> data;

		RGBA sample(Vec2 uv) const;
	};

	std::optional<Image> load_image(const char* image_path);

} // namespace engine
