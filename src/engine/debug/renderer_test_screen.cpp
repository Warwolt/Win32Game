#include <engine/debug/renderer_test_screen.h>

#include <engine/graphics/renderer.h>
#include <engine/input/input.h>

#include <algorithm>
#include <array>
#include <vector>

namespace engine {

	void RendererTestScreen::update(const InputDevices& input) {
		m_window_size = input.window_size;
		m_alpha = (uint8_t)std::clamp((int16_t)m_alpha + 16 * input.mouse.mouse_wheel_delta, 0, 255);
	}

	void RendererTestScreen::draw(Renderer* renderer) const {
		enum class DrawMode {
			Outline,
			Filled,
		};
		std::array<DrawMode, 2> modes = { DrawMode::Outline, DrawMode::Filled };

		const engine::Color color = { 0, 255, 0, m_alpha };
		const int32_t grid_size = 64;
		const int32_t grid_spacing = 16;
		IVec2 grid_pos = { 0, 0 };

		// helper that maps a normalized device coorinate vector into screen
		// coordinates aligned on a grid for nice display.
		auto transform = [grid_size, grid_spacing](Vec2 ndc_vec, IVec2 grid_pos) -> IVec2 {
			IVec2 spacing_offset = grid_spacing * IVec2 { grid_pos.x, grid_pos.y + 1 };
			IVec2 grid_offset = grid_size * grid_pos;
			return spacing_offset + grid_offset +
				IVec2::from_vec2(Vec2 {
					.x = grid_size * (ndc_vec.x + 1.0f) / 2.0f,
					.y = grid_size * (1.0f - ndc_vec.y) / 2.0f,
				});
		};

		auto next_grid_pos = [this, grid_size, grid_spacing](IVec2 pos) {
			int32_t window_grid_width = m_window_size.x / (grid_size + grid_spacing);
			if (pos.x > window_grid_width - 2) {
				return IVec2 { 0, pos.y + 1 };
			}
			return IVec2 { pos.x + 1, pos.y };
		};

		/* Draw pixel */
		{
			renderer->draw_point(transform(Vec2 { 0, 0 }, grid_pos), color);
		}

		/* Draw line */
#pragma region draw line
		// horizontal
		{
			grid_pos = next_grid_pos(grid_pos);
			Vec2 start = { -1.0f, 0.0f };
			Vec2 end = { 1.0f, 0.0f };
			renderer->draw_line(transform(start, grid_pos), transform(end, grid_pos), color);
		}
		// slope -0.5
		{
			grid_pos = next_grid_pos(grid_pos);
			Vec2 start = { -1.0f, 0.5f };
			Vec2 end = { 1.0f, -0.5f };
			renderer->draw_line(transform(start, grid_pos), transform(end, grid_pos), color);
		}
		// slope -1
		{
			grid_pos = next_grid_pos(grid_pos);
			Vec2 start = { -1.0f, 1.0f };
			Vec2 end = { 1.0f, -1.0f };
			renderer->draw_line(transform(start, grid_pos), transform(end, grid_pos), color);
		}
		// slope -2
		{
			grid_pos = next_grid_pos(grid_pos);
			Vec2 start = { -0.5f, 1.0f };
			Vec2 end = { 0.5f, -1.0f };
			renderer->draw_line(transform(start, grid_pos), transform(end, grid_pos), color);
		}
		// slope inf
		{
			grid_pos = next_grid_pos(grid_pos);
			Vec2 start = { 0.0f, -1.0f };
			Vec2 end = { 0.0f, 1.0f };
			renderer->draw_line(transform(start, grid_pos), transform(end, grid_pos), color);
		}
		// slope 2
		{
			grid_pos = next_grid_pos(grid_pos);
			Vec2 start = { -0.5f, -1.0f };
			Vec2 end = { 0.5f, 1.0f };
			renderer->draw_line(transform(start, grid_pos), transform(end, grid_pos), color);
		}
		// slope +1
		{
			grid_pos = next_grid_pos(grid_pos);
			Vec2 start = { -1.0f, -1.0f };
			Vec2 end = { 1.0f, 1.0f };
			renderer->draw_line(transform(start, grid_pos), transform(end, grid_pos), color);
		}
		// slope +0.5
		{
			grid_pos = next_grid_pos(grid_pos);
			Vec2 start = { -1.0f, -0.5f };
			Vec2 end = { 1.0f, 0.5f };
			renderer->draw_line(transform(start, grid_pos), transform(end, grid_pos), color);
		}

		/* Draw rect */
#pragma region draw rect
		// rect
		for (DrawMode mode : modes) {
			grid_pos = next_grid_pos(grid_pos);
			IVec2 pos = transform(Vec2 { -1.0f, 1.0f }, grid_pos);
			Rect rect = {
				.x = pos.x,
				.y = pos.y,
				.width = grid_size,
				.height = grid_size,
			};
			if (mode == DrawMode::Outline) renderer->draw_rect(rect, color);
			if (mode == DrawMode::Filled) renderer->draw_rect_fill(rect, color);
		}

		/* Draw polygon */
#pragma region draw polygon
		// triangle (convex)
		for (DrawMode mode : modes) {
			grid_pos = next_grid_pos(grid_pos);
			std::vector<IVec2> vertices {
				transform(Vec2 { -1.0f, -1.0f }, grid_pos),
				transform(Vec2 { 0.0f, 1.0f }, grid_pos),
				transform(Vec2 { 1.0f, -1.0f }, grid_pos),
			};
			if (mode == DrawMode::Outline) renderer->draw_polygon(vertices, color);
			if (mode == DrawMode::Filled) renderer->draw_polygon_fill(vertices, color);
		}
		// heart (concave)
		for (DrawMode mode : modes) {
			grid_pos = next_grid_pos(grid_pos);
			std::vector<IVec2> vertices {
				transform(Vec2 { -1.0f, 0.5f }, grid_pos),
				transform(Vec2 { -0.5f, 1.0f }, grid_pos),
				transform(Vec2 { 0.0f, 0.5f }, grid_pos),
				transform(Vec2 { 0.5f, 1.0f }, grid_pos),
				transform(Vec2 { 1.0f, 0.5f }, grid_pos),
				transform(Vec2 { 0.0f, -1.0f }, grid_pos),
			};
			if (mode == DrawMode::Outline) renderer->draw_polygon(vertices, color);
			if (mode == DrawMode::Filled) renderer->draw_polygon_fill(vertices, color);
		}
		// mountains (complex)
		for (DrawMode mode : modes) {
			grid_pos = next_grid_pos(grid_pos);
			std::vector<IVec2> vertices {
				transform(Vec2 { -1.0f, -1.0f }, grid_pos),
				transform(Vec2 { -0.5f, 0.5f }, grid_pos),
				transform(Vec2 { 0.0f, -1.0f }, grid_pos),
				transform(Vec2 { 0.5f, 1.0f }, grid_pos),
				transform(Vec2 { 1.0f, -1.0f }, grid_pos),
			};
			if (mode == DrawMode::Outline) renderer->draw_polygon(vertices, color);
			if (mode == DrawMode::Filled) renderer->draw_polygon_fill(vertices, color);
		}

#pragma region draw circle
		// circle
		for (DrawMode mode : modes) {
			grid_pos = next_grid_pos(grid_pos);
			IVec2 center = transform(Vec2 { 0.0f, 0.0f }, grid_pos);
			if (mode == DrawMode::Outline) renderer->draw_circle(center, grid_size / 2, color);
			if (mode == DrawMode::Filled) renderer->draw_circle_fill(center, grid_size / 2, color);
		}
	}

} // namespace engine
