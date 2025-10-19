#pragma once

#include <engine/graphics/font_id.h>
#include <engine/graphics/image_id.h>
#include <engine/graphics/rect.h>
#include <engine/input/time.h>
#include <engine/math/ivec2.h>
#include <engine/debug/test_screens/geometry_test_screen.h>
#include <engine/debug/test_screens/image_test_screen.h>

#include <vector>

namespace engine {

	struct InputDevices;
	class Renderer;
	class ResourceManager;
	class Window;

	struct RenderTestPage {
		enum {
			GeometryTest,
			ImageTest,
			Count,
		};
	};

	class TestScreens {
	public:
		void initialize(ResourceManager* resources);
		void update(const InputDevices& input);
		void draw(Renderer* renderer, IVec2 screen_resolution) const;

	private:
		int m_page = RenderTestPage::GeometryTest;
		GeometryTestScreen m_geometry_test_screen;
		ImageTestScreen m_image_test_screen;
	};

} // namespace engine
