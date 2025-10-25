#pragma once

#include <engine/scene/scene.h>
#include <engine/debug/test_screen/font_test_page.h>
#include <engine/debug/test_screen/geometry_test_page.h>
#include <engine/debug/test_screen/image_test_page.h>

namespace engine {

	struct Input;
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

	// FIXME: class TestScreen : public Scene {
	class TestScreen {
	public:
		TestScreen() = default;
		TestScreen(ResourceManager* resources, int initial_page);
		void update(const Input& input);
		void draw(Renderer* renderer, IVec2 screen_resolution) const;

	private:
		int m_page = 0;
		bool m_just_changed_page = true;
		GeometryTestScreen m_geometry_test_page;
		ImageTestScreen m_image_test_page;
		FontTestScreen m_font_test_page;
	};

} // namespace engine
