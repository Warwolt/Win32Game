#include <engine/graphics/rect.h>

namespace engine {

    bool Rect::empty() const {
        return this->width == 0 && this->height == 0;
    }

} // namespace engine
