#include "screen_stack.h"
#include <engine/ui/screen_stack.h>

namespace engine {

	void ScreenStack::register_screen(std::string screen_name, ScreenConstructor screen_constructor) {
		m_screen_constructors[screen_name] = screen_constructor;
	}

	std::expected<void, ScreenStackError> ScreenStack::push_screen(const std::string& screen_name, ResourceManager* resources) {
		/* Check screen registered */
		auto it = m_screen_constructors.find(screen_name);
		if (it == m_screen_constructors.end()) {
			return std::unexpected(ScreenStackError::InvalidSceeenName);
		}

		/* Show screen if it's not already currently shown */
		if (m_screens.empty() || m_screens.back().screen_name != screen_name) {
			auto& screen_constructor = it->second;
			m_screens.push_back({ screen_name, screen_constructor(resources) });
		}

		return {};
	}

	void ScreenStack::pop_screen() {
		m_screens.pop_back();
	}

	Screen* ScreenStack::current_screen() {
		if (m_screens.empty()) {
			return nullptr;
		}
		return m_screens.back().screen.get();
	}

} // namespace engine
