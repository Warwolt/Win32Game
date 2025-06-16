#pragma once

namespace engine {

    struct ImageID {
        int value;
        bool operator==(const ImageID& rhs) const = default;
    };

    constexpr ImageID INVALID_IMAGE_ID = ImageID(0);

} // namespace engine
