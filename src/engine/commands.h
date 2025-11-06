#pragma once

#include <filesystem>
#include <functional>
#include <memory>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

namespace engine {

	class ResourceManager;
	class SceneManager;
	class SaveFile;
	class ScreenStack;
	class Scene;
	class Screen;
	class Window;

	class CommandList {
	public:
		void run_commands(
			bool* should_quit,
			SaveFile* save_file,
			ResourceManager* resources,
			SceneManager* scene_manager,
			ScreenStack* screen_stack,
			Window* window
		);

		/* App */
		void quit();

		/* File */
		void read_save_file(std::filesystem::path filepath);
		void write_save_file(std::filesystem::path filepath);

		/* SceneManager */
		template <typename SceenType>
		void register_scene() {
			static_assert(std::is_default_constructible<SceenType>::value, "SceenType must be default constructible");
			auto constructor = +[]() { return std::make_unique<SceenType>(); };
			m_commands.push_back(Command_RegisterScene { SceenType::NAME, constructor });
		}
		void load_scene(std::string scene_name);

		/* ScreenStack */
		template <typename ScreenType>
		void register_screen() {
			static_assert(std::is_default_constructible<ScreenType>::value, "ScreenType must be default constructible");
			auto constructor = +[]() { return std::make_unique<ScreenType>(); };
			m_commands.push_back(Command_RegisterScreen { ScreenType::NAME, constructor });
		}
		void push_screen(std::string screen_name);
		void pop_screen();

		/* Window */
		void toggle_fullscreen();
		void set_window_title(std::string window_title);


	private:
		/* App */
		struct Command_Quit {};

		/* File */
		struct Command_ReadSaveFile {
			std::filesystem::path filepath;
		};
		struct Command_WriteSaveFile {
			std::filesystem::path filepath;
		};

		/* SceneManager */
		struct Command_RegisterScene {
			std::string scene_name;
			std::function<std::unique_ptr<Scene>()> scene_constructor;
		};
		struct Command_LoadScene {
			std::string scene_name;
		};

		/* ScreenStack */
		struct Command_RegisterScreen {
			std::string screen_name;
			std::function<std::unique_ptr<Screen>()> screen_constructor;
		};
		struct Command_PushScreen {
			std::string screen_name;
		};
		struct Command_PopScreen {};

		/* Window */
		struct Command_ToggleFullscreen {};
		struct Command_SetWindowTitle {
			std::string window_title;
		};

		using Command = std::variant<
			/* App */
			Command_Quit,

			/* File */
			Command_ReadSaveFile,
			Command_WriteSaveFile,

			/* SceneManager */
			Command_RegisterScene,
			Command_LoadScene,

			/* ScreenStack */
			Command_RegisterScreen,
			Command_PushScreen,
			Command_PopScreen,

			/* Window */
			Command_ToggleFullscreen,
			Command_SetWindowTitle>;

		std::vector<Command> m_commands;
	};

} // namespace engine
