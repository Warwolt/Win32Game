#pragma once

#include <engine/graphics/color.h>
#include <engine/math/vec2.h>
#include <engine/math/ivec2.h>

#include <filesystem>
#include <optional>
#include <vector>

namespace engine {

	struct Image {
		int width = 0;
		int height = 0;
		std::vector<Color> pixels;

		static std::optional<Image> from_path(std::filesystem::path path);
		Color sample(Vec2 uv) const;
		Color get(int x, int y) const;
		IVec2 size() const;
	};

} // namespace engine
