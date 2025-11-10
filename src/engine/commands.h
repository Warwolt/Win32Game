#pragma once

#include <filesystem>
#include <functional>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_set>
#include <variant>
#include <vector>

namespace game {
	struct GameData;
}

namespace engine {

	struct Engine;
	class Scene;
	class Screen;

	class CommandList {
	public:
		void run_commands(Engine* engine);

		/* App */
		void quit();

		/* Input */
		void add_keyboard_binding(std::string action_name, std::unordered_set<uint32_t> keys);

		/* File */
		void load_save_file(std::filesystem::path filepath);
		void write_save_file(std::filesystem::path filepath);

		/* SceneManager */
		template <typename SceenType>
		void register_scene() {
			static_assert(std::is_default_constructible<SceenType>::value, "SceenType must be default constructible");
			auto constructor = +[]() { return std::make_unique<SceenType>(); };
			m_commands.push_back(SceneManagerCommand_RegisterScene { SceenType::NAME, constructor });
		}
		void load_scene(std::string scene_name);

		/* ScreenStack */
		template <typename ScreenType>
		void register_screen() {
			static_assert(std::is_default_constructible<ScreenType>::value, "ScreenType must be default constructible");
			auto constructor = +[]() { return std::make_unique<ScreenType>(); };
			m_commands.push_back(ScreenStackCommand_RegisterScreen { ScreenType::NAME, constructor });
		}
		void push_screen(std::string screen_name);
		void pop_screen();

		/* Window */
		void toggle_fullscreen();
		void set_window_title(std::string window_title);


	private:
		/* App */
		struct AppCommand_Quit {};

		/* Input */
		struct InputCommand_AddKeyboardBinding {
			std::string action_name;
			std::unordered_set<uint32_t> keys;
		};

		/* File */
		struct FileCommand_LoadSaveFile {
			std::filesystem::path filepath;
		};
		struct FileCommand_WriteSaveFile {
			std::filesystem::path filepath;
		};

		/* SceneManager */
		struct SceneManagerCommand_RegisterScene {
			std::string scene_name;
			std::function<std::unique_ptr<Scene>()> scene_constructor;
		};
		struct SceneManagerCommand_LoadScene {
			std::string scene_name;
		};

		/* ScreenStack */
		struct ScreenStackCommand_RegisterScreen {
			std::string screen_name;
			std::function<std::unique_ptr<Screen>()> screen_constructor;
		};
		struct ScreenStackCommand_PushScreen {
			std::string screen_name;
		};
		struct ScreenStackCommand_PopScreen {};

		/* Window */
		struct WindowCommand_ToggleFullscreen {};
		struct WindowCommand_SetWindowTitle {
			std::string window_title;
		};

		using Command = std::variant<
			AppCommand_Quit,

			InputCommand_AddKeyboardBinding,

			FileCommand_LoadSaveFile,
			FileCommand_WriteSaveFile,

			SceneManagerCommand_RegisterScene,
			SceneManagerCommand_LoadScene,

			ScreenStackCommand_RegisterScreen,
			ScreenStackCommand_PushScreen,
			ScreenStackCommand_PopScreen,

			WindowCommand_ToggleFullscreen,
			WindowCommand_SetWindowTitle>;

		std::vector<Command> m_commands;
	};

} // namespace engine
