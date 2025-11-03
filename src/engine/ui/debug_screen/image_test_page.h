#pragma once

#include <engine/animation/animation.h>
#include <engine/graphics/image_id.h>
#include <engine/graphics/rect.h>

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

		AnimationLibrary<SpriteData> m_animation_library;
		AnimationPlayer<SpriteData> m_animation_player;
		AnimationID m_animation_id = {};

		ImageID m_test_image;
		ImageID m_sprite_sheet;
		Rect m_sprite_sheet_size;
	};

} // namespace engine
