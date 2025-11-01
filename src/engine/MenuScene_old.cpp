// TODO:
	// - Screen interface, ScreenStack
	// - MainMenuScreen
	class MenuScene : public engine::Scene {
	public:
		MenuScene(engine::ResourceManager* resources);
		void update(const engine::Input& input, engine::CommandList* commands) override;
		void draw(engine::Renderer* renderer) const override;

	private:
		engine::DebugScreen m_debug_screen;
		int m_cursor = 0;
		int m_screen = ScreenEnum::MAIN_MENU;
	};

	MenuScene::MenuScene(engine::ResourceManager* resources)
		: m_debug_screen(resources, 0) {
	}

	void MenuScene::update(const engine::Input& input, engine::CommandList* commands) {
		/* Main menu screen */
		if (m_screen == ScreenEnum::MAIN_MENU) {
			if (input.keyboard.key_was_pressed_now(VK_ESCAPE)) {
				commands->quit();
			}

			if (input.keyboard.key_was_pressed_now(VK_RETURN)) {
				if (m_cursor == 0) {
					commands->load_scene("GameScene");
				}
				if (m_cursor == 1) {
					// commands->show_screen("debug screen");
					m_screen = ScreenEnum::DEBUG_SCREEN;
				}
				if (m_cursor == 2) {
					commands->quit();
				}
			}

			if (input.keyboard.key_was_pressed_now(VK_DOWN)) {
				m_cursor = (m_cursor + 1) % 3;
			}

			if (input.keyboard.key_was_pressed_now(VK_UP)) {
				m_cursor = (3 + m_cursor - 1) % 3;
			}
		}

		/* Debug screen */
		if (m_screen == ScreenEnum::DEBUG_SCREEN) {
			m_debug_screen.update(input, commands);

			if (input.keyboard.key_was_pressed_now(VK_ESCAPE)) {
				// commands->close_current_screen();
				m_screen = ScreenEnum::MAIN_MENU;
			}
		}
	}

	void MenuScene::draw(engine::Renderer* renderer) const {
		/* Main menu screen */
		if (m_screen == ScreenEnum::MAIN_MENU) {
			engine::IVec2 screen_resolution = renderer->screen_resolution();
			engine::Rect header_box = { .x = screen_resolution.x / 2 - 36, .y = screen_resolution.y / 4 };

			/* Header */
			renderer->draw_text(engine::DEFAULT_FONT_ID, 16, header_box, engine::RGBA::white(), "Main Menu");

			/* Menu items */
			renderer->draw_text(engine::DEFAULT_FONT_ID, 16, header_box + engine::IVec2 { 16, screen_resolution.y / 4 }, engine::RGBA::white(), "Play");
			renderer->draw_text(engine::DEFAULT_FONT_ID, 16, header_box + engine::IVec2 { 16, screen_resolution.y / 4 + 16 }, engine::RGBA::white(), "Debug");
			renderer->draw_text(engine::DEFAULT_FONT_ID, 16, header_box + engine::IVec2 { 16, screen_resolution.y / 4 + 32 }, engine::RGBA::white(), "Quit");

			/* Cursor */
			renderer->draw_text(engine::DEFAULT_FONT_ID, 16, header_box + engine::IVec2 { 0, screen_resolution.y / 4 + m_cursor * 16 }, engine::RGBA::white(), ">");
		}

		/* Debug screen */
		if (m_screen == ScreenEnum::DEBUG_SCREEN) {
			m_debug_screen.draw(renderer);
		}
	}
