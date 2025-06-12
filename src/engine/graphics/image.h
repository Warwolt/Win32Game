#pragma once

#include <engine/graphics/rgba.h>

#include <optional>

namespace engine {

	struct Image {
		int width = 0;
		int height = 0;
        RGBA* data;
	};

    std::optional<Image> load_image(const char* image_path);
    void free_image(const Image& image);

} // namespace engine
