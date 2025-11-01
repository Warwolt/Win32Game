#pragma once

#include <engine/ui/screen.h>
#include <engine/ui/debug_screen/font_test_page.h>
#include <engine/ui/debug_screen/geometry_test_page.h>
#include <engine/ui/debug_screen/image_test_page.h>

namespace engine {

	struct Input;
	class Renderer;
	class ResourceManager;
	class Window;

	struct DebugScreenPage {
		enum {
			GeometryTest,
			ImageTest,
			FontTest,
			Count,
		};
	};

	class DebugScreen : public Screen {
	public:
		static constexpr char NAME[] = "DebugScreen";
		void initialize(ResourceManager* resources, CommandList* commands) override;
		void update(const Input& input, CommandList* commands) override;
		void draw(Renderer* renderer) const override;

	private:
		int m_page = 0;
		bool m_just_changed_page = true;
		GeometryTestPage m_geometry_test_page;
		ImageDebugPage m_image_test_page;
		FontDebugPage m_font_test_page;
	};

} // namespace engine
