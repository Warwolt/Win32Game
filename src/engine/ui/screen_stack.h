#pragma once

#include <engine/ui/screen.h>

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace engine {

	enum class ScreenStackError {
		InvalidSceeenName,
	};

	class ScreenStack {
	public:
		using ScreenConstructor = std::function<std::unique_ptr<Screen>()>;

		void HOT_RELOAD_unregister_all_screens();
		void HOT_RELOAD_patch_vtables();

		template <typename ScreenType>
		void register_screen() {
			static_assert(std::is_default_constructible<ScreenType>::value, "ScreenType must be default constructible");
			m_screen_constructors[ScreenType::NAME] = +[]() { return std::make_unique<ScreenType>(); };
		}

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
