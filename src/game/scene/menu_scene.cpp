#include <game/scene/menu_scene.h>

#include <game/ui/main_menu.h>

#include <engine/commands.h>
#include <engine/graphics/renderer.h>

namespace game {

	void MenuScene::initialize(const engine::SaveFile& /*save_file*/, engine::ResourceManager* /*resources*/, engine::CommandList* commands) {
		commands->push_screen(MainMenu::NAME);
	}

	void MenuScene::update(const engine::Input& /*input*/, engine::CommandList* /*commands*/) {
	}

	void MenuScene::draw(engine::Renderer* renderer) const {
		renderer->clear_screen(engine::RGBA::black());
	}

} // namespace game
