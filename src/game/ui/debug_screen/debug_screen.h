#pragma once

#include <engine/ui/screen.h>
#include <game/ui/debug_screen/font_test_page.h>
#include <game/ui/debug_screen/geometry_test_page.h>
#include <game/ui/debug_screen/image_test_page.h>

namespace engine {
	struct Input;
	class Renderer;
	class ResourceManager;
	class Window;
}

namespace game {

	struct DebugScreenPage {
		enum {
			GeometryTest,
			ImageTest,
			FontTest,
			Count,
		};
	};

	class DebugScreen : public engine::Screen {
	public:
		static constexpr char NAME[] = "DebugScreen";
		void initialize(GameData* game, engine::ResourceManager* resources, engine::CommandList* commands) override;
		void update(GameData* game, const engine::Input& input, engine::CommandList* commands) override;
		void draw(const GameData& game, engine::Renderer* renderer) const override;

	private:
		int m_page = 0;
		bool m_just_changed_page = true;
		GeometryTestPage m_geometry_test_page;
		ImageDebugPage m_image_test_page;
		FontDebugPage m_font_test_page;
	};

} // namespace engine
