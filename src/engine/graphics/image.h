#pragma once

#include <engine/graphics/rgba.h>
#include <engine/math/vec2.h>

#include <filesystem>
#include <optional>
#include <vector>

namespace engine {

	struct Image {
		int width = 0;
		int height = 0;
		std::vector<RGBA> pixels;

		static std::optional<Image> from_path(std::filesystem::path path);
		RGBA sample(Vec2 uv) const;
		RGBA get(int x, int y) const;
	};

} // namespace engine
