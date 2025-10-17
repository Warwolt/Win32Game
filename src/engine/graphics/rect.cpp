#include <engine/graphics/rect.h>

namespace engine {

	bool Rect::empty() const {
		return this->width == 0 && this->height == 0;
	}

	IVec2 Rect::pos() const {
		return IVec2 { this->x, this->y };
	}

} // namespace engine
