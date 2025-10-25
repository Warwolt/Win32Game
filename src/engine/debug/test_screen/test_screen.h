#pragma once

#include <engine/debug/test_screen/geometry_test_page.h>
#include <engine/debug/test_screen/image_test_page.h>
#include <engine/debug/test_screen/font_test_page.h>

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

	class TestScreen {
	public:
		void initialize(ResourceManager* resources, int initial_page);
		void update(const InputDevices& input);
		void draw(Renderer* renderer, IVec2 screen_resolution) const;

	private:
		int m_page = 0;
		bool m_just_changed_page = true;
		GeometryTestScreen m_geometry_test_page;
		ImageTestScreen m_image_test_page;
		FontTestScreen m_font_test_page;
	};

} // namespace engine
