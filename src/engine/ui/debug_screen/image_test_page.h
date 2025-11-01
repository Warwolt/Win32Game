#pragma once

#include <engine/graphics/image_id.h>
#include <engine/graphics/rect.h>
#include <engine/input/time.h>
#include <engine/animation/animation_system.h>

#include <vector>

namespace engine {

	struct Input;
	class Renderer;
	class ResourceManager;

	class ImageDebugPage {
	public:
		void initialize(ResourceManager* resources);
		void update(bool opened_now, const Input& input);
		void draw(Renderer* renderer) const;

	private:
		struct SpriteData {
			int index;
			bool is_flipped;
		};

		AnimationSystem<SpriteData> m_animation_system;

		ImageID m_test_image;
		ImageID m_sprite_sheet;
		Rect m_sprite_sheet_size;
	};

} // namespace engine
