#pragma once

#include <engine/ui/screen.h>

#include <expected>
#include <functional>
#include <memory>
#include <string>
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

		void register_screen(std::string screen_name, ScreenConstructor screen_constructor);
		std::expected<void, ScreenStackError> show_screen(const std::string& screen_name, ResourceManager* resources);
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
