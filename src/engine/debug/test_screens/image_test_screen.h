#pragma once

#include <engine/graphics/image_id.h>
#include <engine/graphics/rect.h>
#include <engine/input/time.h>
#include <engine/math/ivec2.h>

#include <vector>

namespace engine {

	struct InputDevices;
	class Renderer;
	class ResourceManager;

	class ImageTestScreen {
	public:
		void initialize(ResourceManager* resources);
		void update(const InputDevices& input);
		void draw(Renderer* renderer, IVec2 screen_resolution) const;

	private:
		struct AnimationFrame {
			int index;
			bool flipped;
		};

		ImageID m_clipping_image;
		ImageID m_transparency_image;
		ImageID m_sprite_sheet;
		int m_animation_index = 0;
		Rect m_sprite_sheet_size;
		std::vector<AnimationFrame> m_animation_frames;
		Time m_last_sprite_sheet_frame;
	};

} // namespace engine
