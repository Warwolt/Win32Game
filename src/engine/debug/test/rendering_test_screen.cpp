#include <engine/debug/test/rendering_test_screen.h>

#include <engine/graphics/renderer.h>
#include <engine/graphics/window.h>
#include <engine/input/input.h>

#include <algorithm>
#include <array>
#include <vector>

namespace engine {

	void RenderingTestScreen::update(const InputDevices& input) {
		m_alpha = (uint8_t)std::clamp((int16_t)m_alpha + 16 * input.mouse.mouse_wheel_delta, 0, 255);
	}

	void RenderingTestScreen::draw(Renderer* renderer, IVec2 screen_resolution) const {
		enum class FillMode {
			Outline,
			Filled,
		};
		enum class ColorMode {
			Mono,
			Gradient,
		};
		std::array<FillMode, 2> fill_modes = { FillMode::Outline, FillMode::Filled };
		std::array<ColorMode, 2> color_modes = { ColorMode::Mono, ColorMode::Gradient };

		const engine::RGBA color = { 0, 255, 0, m_alpha };
		const int32_t grid_size = 64;
		const int32_t grid_spacing = 16;
		IVec2 grid_pos = { -1, 0 };

		auto next_grid_pos = [screen_resolution, grid_size, grid_spacing](IVec2 pos) {
			int32_t window_grid_width = screen_resolution.x / (grid_size + grid_spacing);
			if (pos.x > window_grid_width - 2) {
				return IVec2 { 0, pos.y + 1 };
			}
			return IVec2 { pos.x + 1, pos.y };
		};

		// helper that maps a normalized device coorinate vector into screen
		// coordinates aligned on a grid for nice display.
		auto get_pos = [grid_size, grid_spacing](Vec2 ndc_vec, IVec2 grid_pos) -> IVec2 {
			IVec2 spacing_offset = grid_spacing * IVec2 { grid_pos.x, grid_pos.y + 1 };
			IVec2 grid_offset = grid_size * grid_pos;
			return spacing_offset + grid_offset +
				IVec2::from_vec2(Vec2 {
					.x = grid_size * (ndc_vec.x + 1.0f) / 2.0f,
					.y = grid_size * (1.0f - ndc_vec.y) / 2.0f,
				});
		};

		auto get_color = [this](RGBA color, ColorMode mode) {
			return mode == ColorMode::Mono ? RGBA { 0, 255, 0, m_alpha } : color;
		};

		/* Draw line */
#pragma region draw line
		for (ColorMode color_mode : color_modes) {
			// horizontal
			{
				grid_pos = next_grid_pos(grid_pos);
				Vertex start = { .pos = get_pos({ -1.0f, 0.0f }, grid_pos), .color = get_color({ 255, 0, 0, m_alpha }, color_mode) };
				Vertex end = { .pos = get_pos({ 1.0f, 0.0f }, grid_pos), .color = get_color({ 0, 0, 255, m_alpha }, color_mode) };
				renderer->draw_line(start, end);
			}

			// slope -0.5
			{
				grid_pos = next_grid_pos(grid_pos);
				Vertex start = { .pos = get_pos({ -1.0f, 0.5f }, grid_pos), .color = get_color({ 255, 0, 0, m_alpha }, color_mode) };
				Vertex end = { .pos = get_pos({ 1.0f, -0.5f }, grid_pos), .color = get_color({ 0, 0, 255, m_alpha }, color_mode) };
				renderer->draw_line(start, end);
			}

			// slope -1
			{
				grid_pos = next_grid_pos(grid_pos);
				Vertex start = { .pos = get_pos({ -1.0f, 1.0f }, grid_pos), .color = get_color({ 255, 0, 0, m_alpha }, color_mode) };
				Vertex end = { .pos = get_pos({ 1.0f, -1.0f }, grid_pos), .color = get_color({ 0, 0, 255, m_alpha }, color_mode) };
				renderer->draw_line(start, end);
			}

			// slope -2
			{
				grid_pos = next_grid_pos(grid_pos);
				Vertex start = { .pos = get_pos({ -0.5f, 1.0f }, grid_pos), .color = get_color({ 255, 0, 0, m_alpha }, color_mode) };
				Vertex end = { .pos = get_pos({ 0.5f, -1.0f }, grid_pos), .color = get_color({ 0, 0, 255, m_alpha }, color_mode) };
				renderer->draw_line(start, end);
			}

			// slope inf
			{
				grid_pos = next_grid_pos(grid_pos);
				Vertex start = { .pos = get_pos({ 0.0f, 1.0f }, grid_pos), .color = get_color({ 255, 0, 0, m_alpha }, color_mode) };
				Vertex end = { .pos = get_pos({ 0.0f, -1.0f }, grid_pos), .color = get_color({ 0, 0, 255, m_alpha }, color_mode) };
				renderer->draw_line(start, end);
			}

			// slope +2
			{
				grid_pos = next_grid_pos(grid_pos);
				Vertex start = { .pos = get_pos({ 0.5f, 1.0f }, grid_pos), .color = get_color({ 255, 0, 0, m_alpha }, color_mode) };
				Vertex end = { .pos = get_pos({ -0.5f, -1.0f }, grid_pos), .color = get_color({ 0, 0, 255, m_alpha }, color_mode) };
				renderer->draw_line(start, end);
			}
			// slope +1
			{
				grid_pos = next_grid_pos(grid_pos);
				Vertex start = { .pos = get_pos({ 1.0f, 1.0f }, grid_pos), .color = get_color({ 255, 0, 0, m_alpha }, color_mode) };
				Vertex end = { .pos = get_pos({ -1.0f, -1.0f }, grid_pos), .color = get_color({ 0, 0, 255, m_alpha }, color_mode) };
				renderer->draw_line(start, end);
			}
			// slope +0.5
			{
				grid_pos = next_grid_pos(grid_pos);
				Vertex start = { .pos = get_pos({ 1.0f, 0.5f }, grid_pos), .color = get_color({ 255, 0, 0, m_alpha }, color_mode) };
				Vertex end = { .pos = get_pos({ -1.0f, -0.5f }, grid_pos), .color = get_color({ 0, 0, 255, m_alpha }, color_mode) };
				renderer->draw_line(start, end);
			}
		}

		/* Draw rect */
#pragma region draw rect
		// rect
		for (FillMode mode : fill_modes) {
			grid_pos = next_grid_pos(grid_pos);
			IVec2 pos = get_pos(Vec2 { -1.0f, 1.0f }, grid_pos);
			Rect rect = {
				.x = pos.x,
				.y = pos.y,
				.width = grid_size,
				.height = grid_size,
			};
			if (mode == FillMode::Outline) renderer->draw_rect(rect, color);
			if (mode == FillMode::Filled) renderer->draw_rect_fill(rect, color);
		}

		/* Draw circle */
#pragma region draw circle
		// circle
		for (FillMode mode : fill_modes) {
			grid_pos = next_grid_pos(grid_pos);
			IVec2 center = get_pos(Vec2 { 0.0f, 0.0f }, grid_pos);
			if (mode == FillMode::Outline) renderer->draw_circle(center, grid_size / 2, color);
			if (mode == FillMode::Filled) renderer->draw_circle_fill(center, grid_size / 2, color);
		}
	}

	/* Draw polygon */
#pragma region draw polygon
	// TODO: draw some polygons

} // namespace engine
