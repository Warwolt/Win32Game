#pragma once

#include <engine/ui/screen.h>

#include <optional>
#include <functional>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace engine {

	enum class ScreenStackError {
		InvalidSceeenName,
	};

	class ScreenStack {
	public:
		using ScreenConstructor = std::function<std::unique_ptr<Screen>()>;

		void register_screen(std::string name, ScreenConstructor constructor);
		Screen* top_screen();
		std::optional<ScreenStackError> push_screen(const std::string& screen_name);
		void pop_screen();
		void clear();

	private:
		struct ScreenStackItem {
			std::string screen_name;
			std::unique_ptr<Screen> screen;
		};

		std::vector<ScreenStackItem> m_screens;
		std::unordered_map<std::string, ScreenConstructor> m_screen_constructors;
	};

} // namespace engine
