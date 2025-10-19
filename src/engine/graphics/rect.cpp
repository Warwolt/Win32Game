#include <engine/graphics/rect.h>

namespace engine {

	Rect operator+(Rect lhs, IVec2 rhs) {
		return Rect { lhs.x + rhs.x, lhs.y + rhs.y, lhs.width, lhs.height };
	}

	bool Rect::empty() const {
		return this->width == 0 && this->height == 0;
	}

	IVec2 Rect::pos() const {
		return IVec2 { this->x, this->y };
	}

} // namespace engine
