#include <game/scene/menu_scene.h>

#include <game/ui/main_menu.h>
#include <engine/debug/logging.h>

#include <engine/commands.h>

namespace game {

	void MenuScene::initialize(engine::ResourceManager* /*resources*/, engine::CommandList* commands) {
		LOG_DEBUG("MenuScene::initialize");
		commands->push_screen(MainMenu::NAME);
	}

	void MenuScene::update(const engine::Input& /*input*/, engine::CommandList* /*commands*/) {
	}

	void MenuScene::draw(engine::Renderer* /*renderer*/) const {
	}

} // namespace game
