#include <engine/ui/screen_stack.h>

namespace engine {

	void ScreenStack::HOT_RELOAD_unregister_all_screens() {
		m_screen_constructors.clear();
	}

	void ScreenStack::HOT_RELOAD_patch_vtables() {
		// 2025-11-11
		// Because hot reloading the DLL might move where code is in memory, we
		// have to patch the vtable so that the function pointers point to the
		// current code locations, otherwise we might crash.
		for (const ScreenStackItem& stack_entry : m_screens) {
			if (auto it = m_screen_constructors.find(stack_entry.screen_name); it != m_screen_constructors.end()) {
				auto& screen_constructor = it->second;
				std::unique_ptr<Screen> dummy_screen = screen_constructor();
				// Assume that __vfptr member is first member of Screen
				void** current_vfptr = (void**)stack_entry.screen.get();
				void** new_vfptr = (void**)dummy_screen.get();
				*current_vfptr = *new_vfptr;
			}
		}
	}

	Screen* ScreenStack::top_screen() {
		if (m_screens.empty()) {
			return nullptr;
		}
		return m_screens.back().screen.get();
	}

	std::optional<ScreenStackError> ScreenStack::push_screen(const std::string& screen_name) {
		/* Check screen registered */
		auto it = m_screen_constructors.find(screen_name);
		if (it == m_screen_constructors.end()) {
			return ScreenStackError::InvalidSceeenName;
		}

		/* Show screen if it's not already currently shown */
		if (m_screens.empty() || m_screens.back().screen_name != screen_name) {
			auto& screen_constructor = it->second;
			m_screens.push_back({ screen_name, screen_constructor() });
		}

		return {};
	}

	void ScreenStack::pop_screen() {
		m_screens.pop_back();
	}

	void ScreenStack::clear() {
		m_screens.clear();
	}

} // namespace engine
