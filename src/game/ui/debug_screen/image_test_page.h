#pragma once

#include <engine/animation/animation.h>
#include <engine/graphics/image_id.h>
#include <engine/graphics/rect.h>

namespace engine {
	struct Input;
	class Renderer;
	class ResourceManager;
}

namespace game {

	class ImageDebugPage {
	public:
		void initialize(engine::ResourceManager* resources);
		void update(bool opened_now, const engine::Input& input);
		void draw(engine::Renderer* renderer) const;

	private:
		struct SpriteData {
			int index;
			bool is_flipped;
		};

		engine::AnimationLibrary<SpriteData> m_animation_library;
		engine::AnimationPlayer<SpriteData> m_animation_player;
		engine::AnimationID m_animation_id = {};

		engine::ImageID m_test_image;
		engine::ImageID m_sprite_sheet;
		engine::Rect m_sprite_sheet_size;
	};

} // namespace engine
