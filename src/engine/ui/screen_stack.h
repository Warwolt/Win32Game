#pragma once

#include <engine/ui/screen.h>

#include <expected>
#include <functional>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>


namespace engine {

	class ResourceManager;

	enum class ScreenStackError {
		InvalidSceeenName,
	};

	class ScreenStack {
	public:
		using ScreenConstructor = std::function<std::unique_ptr<Screen>(ResourceManager*)>;

		template <typename SceneType>
		void register_screen() {
			static_assert(std::constructible_from<SceneType, ResourceManager*>, "Scene must have a constructor that accepts a ResourceManager, but none was found.");
			m_screen_constructors[SceneType::NAME] = [](ResourceManager* resources) { return std::make_unique<SceneType>(resources); };
		}

		std::expected<void, ScreenStackError> push_screen(const std::string& screen_name, ResourceManager* resources);
		void pop_screen();
		Screen* current_screen();

	private:
		struct ScreenStackItem {
			std::string screen_name;
			std::unique_ptr<Screen> screen;
		};

		std::vector<ScreenStackItem> m_screens;
		std::unordered_map<std::string, ScreenConstructor> m_screen_constructors;
	};

} // namespace engine
