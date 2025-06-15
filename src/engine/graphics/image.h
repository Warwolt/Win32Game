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

		static std::optional<Image> from_path(const char* path);
		RGBA sample(Vec2 uv) const;
	};

} // namespace engine
