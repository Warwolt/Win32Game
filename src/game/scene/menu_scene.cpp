#include <game/scene/menu_scene.h>

#include <game/ui/main_menu.h>

#include <engine/commands.h>
#include <engine/file/resource_manager.h>
#include <engine/graphics/font.h>
#include <engine/graphics/renderer.h>

namespace game {

	void MenuScene::initialize(GameData* /*game*/, engine::ResourceManager* resources, engine::CommandList* commands) {
		resources->typeface(engine::DEFAULT_FONT_ID).add_font(16);
		resources->typeface(engine::DEFAULT_FONT_ID).add_font(32);
		commands->push_screen(MainMenu::NAME);
	}

	void MenuScene::update(GameData* /*game*/, const engine::Input& /*input*/, engine::CommandList* /*commands*/) {
	}

	void MenuScene::draw(const GameData& /*game*/, engine::Renderer* renderer) const {
		renderer->clear_screen(engine::RGBA::black());
	}

} // namespace game
