#include <game/ui/debug_screen/geometry_test_page.h>

#include <engine/debug/profiling.h>
#include <engine/file/resource_manager.h>
#include <engine/graphics/renderer.h>
#include <engine/input/input.h>
#include <engine/input/keyboard.h>

#include <windows.h>

#include <algorithm>
#include <array>
#include <format>

namespace game {

	void GeometryTestPage::update(const engine::Input& input) {
		m_alpha = (uint8_t)std::clamp(m_alpha + 16 * input.mouse.mouse_wheel_delta, 0, 255);
	}

	void GeometryTestPage::draw(engine::Renderer* renderer, engine::IVec2 screen_resolution) const {
		CPUProfilingScope_Engine();
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
		const int32_t grid_size = 32;
		const int32_t grid_spacing = 8;
		engine::IVec2 grid_pos = { -1, 0 };

		auto next_grid_pos = [screen_resolution, grid_size, grid_spacing](engine::IVec2 pos) {
			int32_t window_grid_width = screen_resolution.x / (grid_size + grid_spacing);
			if (pos.x > window_grid_width - 2) {
				return engine::IVec2 { 0, pos.y + 1 };
			}
			return engine::IVec2 { pos.x + 1, pos.y };
		};

		// helper that maps a normalized device coordinate vector into screen
		// coordinates aligned on a grid for nice display.
		auto get_pos = [grid_size, grid_spacing](engine::Vec2 ndc_vec, engine::IVec2 grid_pos) -> engine::IVec2 {
			engine::IVec2 spacing_offset = grid_spacing * engine::IVec2 { grid_pos.x, grid_pos.y + 1 };
			engine::IVec2 grid_offset = grid_size * grid_pos;
			int y_offset = 16;
			return spacing_offset + grid_offset +
				engine::IVec2::from_vec2(engine::Vec2 {
					.x = grid_size * (ndc_vec.x + 1.0f) / 2.0f,
					.y = y_offset + grid_size * (1.0f - ndc_vec.y) / 2.0f,
				});
		};

		auto get_color = [this](engine::RGBA color, ColorMode mode) {
			return mode == ColorMode::Mono ? engine::RGBA { 0, 255, 0, m_alpha } : color;
		};

		/* Draw line */
		for (ColorMode color_mode : color_modes) {
			// horizontal
			{
				RENDERER_LOG(renderer, std::format("Draw horizontal line ({})", color_mode == ColorMode::Gradient ? "Gradient" : "Mono"));
				grid_pos = next_grid_pos(grid_pos);
				engine::Vertex start = { .pos = get_pos({ -1.0f, 0.0f }, grid_pos), .color = get_color({ 255, 0, 0, m_alpha }, color_mode) };
				engine::Vertex end = { .pos = get_pos({ 1.0f, 0.0f }, grid_pos), .color = get_color({ 0, 0, 255, m_alpha }, color_mode) };
				renderer->draw_line(start, end);
			}

			// slope -0.5
			{
				RENDERER_LOG(renderer, std::format("Draw slope -0.5 line ({})", color_mode == ColorMode::Gradient ? "Gradient" : "Mono"));
				grid_pos = next_grid_pos(grid_pos);
				engine::Vertex start = { .pos = get_pos({ -1.0f, 0.5f }, grid_pos), .color = get_color({ 255, 0, 0, m_alpha }, color_mode) };
				engine::Vertex end = { .pos = get_pos({ 1.0f, -0.5f }, grid_pos), .color = get_color({ 0, 0, 255, m_alpha }, color_mode) };
				renderer->draw_line(start, end);
			}

			// slope -1
			{
				RENDERER_LOG(renderer, std::format("Draw slope -1 line ({})", color_mode == ColorMode::Gradient ? "Gradient" : "Mono"));
				grid_pos = next_grid_pos(grid_pos);
				engine::Vertex start = { .pos = get_pos({ -1.0f, 1.0f }, grid_pos), .color = get_color({ 255, 0, 0, m_alpha }, color_mode) };
				engine::Vertex end = { .pos = get_pos({ 1.0f, -1.0f }, grid_pos), .color = get_color({ 0, 0, 255, m_alpha }, color_mode) };
				renderer->draw_line(start, end);
			}

			// slope -2
			{
				RENDERER_LOG(renderer, std::format("Draw slope -2 line ({})", color_mode == ColorMode::Gradient ? "Gradient" : "Mono"));
				grid_pos = next_grid_pos(grid_pos);
				engine::Vertex start = { .pos = get_pos({ -0.5f, 1.0f }, grid_pos), .color = get_color({ 255, 0, 0, m_alpha }, color_mode) };
				engine::Vertex end = { .pos = get_pos({ 0.5f, -1.0f }, grid_pos), .color = get_color({ 0, 0, 255, m_alpha }, color_mode) };
				renderer->draw_line(start, end);
			}

			// slope inf
			{
				RENDERER_LOG(renderer, std::format("Draw slope inf line ({})", color_mode == ColorMode::Gradient ? "Gradient" : "Mono"));
				grid_pos = next_grid_pos(grid_pos);
				engine::Vertex start = { .pos = get_pos({ 0.0f, 1.0f }, grid_pos), .color = get_color({ 255, 0, 0, m_alpha }, color_mode) };
				engine::Vertex end = { .pos = get_pos({ 0.0f, -1.0f }, grid_pos), .color = get_color({ 0, 0, 255, m_alpha }, color_mode) };
				renderer->draw_line(start, end);
			}

			// slope +2
			{
				RENDERER_LOG(renderer, std::format("Draw slope +2 line ({})", color_mode == ColorMode::Gradient ? "Gradient" : "Mono"));
				grid_pos = next_grid_pos(grid_pos);
				engine::Vertex start = { .pos = get_pos({ 0.5f, 1.0f }, grid_pos), .color = get_color({ 255, 0, 0, m_alpha }, color_mode) };
				engine::Vertex end = { .pos = get_pos({ -0.5f, -1.0f }, grid_pos), .color = get_color({ 0, 0, 255, m_alpha }, color_mode) };
				renderer->draw_line(start, end);
			}
		}

		/* Draw rect */
		// rect
		for (FillMode mode : fill_modes) {
			RENDERER_LOG(renderer, std::format("Draw rect ({})", mode == FillMode::Outline ? "Outline" : "Filled"));
			grid_pos = next_grid_pos(grid_pos);
			engine::IVec2 pos = get_pos(engine::Vec2 { -1.0f, 1.0f }, grid_pos);
			engine::Rect rect = {
				.x = pos.x,
				.y = pos.y,
				.width = grid_size,
				.height = grid_size,
			};
			if (mode == FillMode::Outline) renderer->draw_rect(rect, color);
			if (mode == FillMode::Filled) renderer->draw_rect_fill(rect, color);
		}

		/* Draw circle */
		// circle
		for (FillMode mode : fill_modes) {
			RENDERER_LOG(renderer, std::format("Draw circle ({})", mode == FillMode::Outline ? "Outline" : "Filled"));
			grid_pos = next_grid_pos(grid_pos);
			engine::IVec2 center = get_pos(engine::Vec2 { 0.0f, 0.0f }, grid_pos);
			if (mode == FillMode::Outline) renderer->draw_circle(center, grid_size / 2, color);
			if (mode == FillMode::Filled) renderer->draw_circle_fill(center, grid_size / 2, color);
		}

		/* Draw triangle */
		// isoceles triangle pointing up
		for (ColorMode color_mode : color_modes) {
			for (FillMode mode : fill_modes) {
				RENDERER_LOG(renderer, std::format("Draw isoceles triangle pointing up ({}, {})", color_mode == ColorMode::Gradient ? "Gradient" : "Mono", mode == FillMode::Outline ? "Outline" : "Filled"));
				grid_pos = next_grid_pos(grid_pos);
				engine::Vertex left = { .pos = get_pos(engine::Vec2 { -1.0f, -1.0f }, grid_pos), .color = get_color({ 255, 0, 0, m_alpha }, color_mode) };
				engine::Vertex top = { .pos = get_pos(engine::Vec2 { 0.0f, 1.0f }, grid_pos), .color = get_color({ 0, 255, 0, m_alpha }, color_mode) };
				engine::Vertex right = { .pos = get_pos(engine::Vec2 { 1.0f, -1.0f }, grid_pos), .color = get_color({ 0, 0, 255, m_alpha }, color_mode) };
				if (mode == FillMode::Outline) renderer->draw_triangle(left, top, right);
				if (mode == FillMode::Filled) renderer->draw_triangle_fill(left, top, right);
			}
		}

		// isoceles triangle pointing up
		for (ColorMode color_mode : color_modes) {
			for (FillMode mode : fill_modes) {
				RENDERER_LOG(renderer, std::format("Draw isoceles triangle pointing right ({}, {})", color_mode == ColorMode::Gradient ? "Gradient" : "Mono", mode == FillMode::Outline ? "Outline" : "Filled"));
				grid_pos = next_grid_pos(grid_pos);
				engine::Vertex left = { .pos = get_pos(engine::Vec2 { -1.0f, -1.0f }, grid_pos), .color = get_color({ 255, 0, 0, m_alpha }, color_mode) };
				engine::Vertex top = { .pos = get_pos(engine::Vec2 { -1.0f, 1.0f }, grid_pos), .color = get_color({ 0, 255, 0, m_alpha }, color_mode) };
				engine::Vertex right = { .pos = get_pos(engine::Vec2 { 1.0f, 0.0f }, grid_pos), .color = get_color({ 0, 0, 255, m_alpha }, color_mode) };
				if (mode == FillMode::Outline) renderer->draw_triangle(left, top, right);
				if (mode == FillMode::Filled) renderer->draw_triangle_fill(left, top, right);
			}
		}

		// scalene triangle
		for (ColorMode color_mode : color_modes) {
			for (FillMode mode : fill_modes) {
				RENDERER_LOG(renderer, std::format("Draw scelene triangle ({}, {})", color_mode == ColorMode::Gradient ? "Gradient" : "Mono", mode == FillMode::Outline ? "Outline" : "Filled"));
				grid_pos = next_grid_pos(grid_pos);
				engine::Vertex left = { .pos = get_pos(engine::Vec2 { -0.5f, -1.0f }, grid_pos), .color = get_color({ 255, 0, 0, m_alpha }, color_mode) };
				engine::Vertex top = { .pos = get_pos(engine::Vec2 { 0.0f, 1.0f }, grid_pos), .color = get_color({ 0, 255, 0, m_alpha }, color_mode) };
				engine::Vertex right = { .pos = get_pos(engine::Vec2 { 1.0f, -0.5f }, grid_pos), .color = get_color({ 0, 0, 255, m_alpha }, color_mode) };
				if (mode == FillMode::Outline) renderer->draw_triangle(left, top, right);
				if (mode == FillMode::Filled) renderer->draw_triangle_fill(left, top, right);
			}
		}
	}

} // namespace engine
