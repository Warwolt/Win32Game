#include <engine/graphics/image.h>

#include <stb_image/stb_image.h>

namespace engine {

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
