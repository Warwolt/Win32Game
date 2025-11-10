#pragma once

#include <engine/math/ivec2.h>

#include <stdint.h>

namespace engine {
	struct Input;
	class Renderer;
}

namespace game {

	class GeometryTestPage {
	public:
		void update(const engine::Input& input);
		void draw(engine::Renderer* renderer, engine::IVec2 screen_resolution) const;

	private:
		uint8_t m_alpha = 255;
	};

} // namespace game
