#pragma once

#include <engine/debug/test_screens/geometry_test_screen.h>
#include <engine/debug/test_screens/image_test_screen.h>
#include <engine/debug/test_screens/font_test_screen.h>

namespace engine {

	struct InputDevices;
	class Renderer;
	class ResourceManager;
	class Window;

	struct TestScreenPage {
		enum {
			GeometryTest,
			ImageTest,
			FontTest,
			Count,
		};
	};

	class TestScreens {
	public:
		void initialize(ResourceManager* resources, int initial_page);
		void update(const InputDevices& input);
		void draw(Renderer* renderer, IVec2 screen_resolution) const;

	private:
		int m_page = 0;
		bool m_just_changed_page = true;
		GeometryTestScreen m_geometry_test_screen;
		ImageTestScreen m_image_test_screen;
		FontTestScreen m_font_test_screen;
	};

} // namespace engine
