#pragma once

#include "utils.hpp"
#include "color.hpp"

#include <vector>
#include <string>

class Canvas final
{
public:

    explicit Canvas(const Size2D dimensions, std::string name = "canvas", const Color& clearColor = Color::kBlack)
        : m_dimensions{ dimensions }
        , m_name{ std::move(name) }
    {
        assert(m_dimensions.is_valid());

        m_pixel_buffer.resize(
                static_cast<std::size_t>(m_dimensions.width * m_dimensions.height),
                Color::to_rgba_u8(clearColor));
    }

    // Canvas origin (0,0) is at the bottom-left corner.
    auto draw_pixel(const Point2D point, const Color& color) -> void
    {
        if (point.x < 0 || point.x >= m_dimensions.width ||
            point.y < 0 || point.y >= m_dimensions.height) [[unlikely]]
        {
            return;
        }

        const auto pixel_idx = static_cast<std::size_t>(point.x + (point.y * m_dimensions.width));
        assert(pixel_idx < m_pixel_buffer.size());

        const Color sourceColor  = Color::from_rgba_u8(m_pixel_buffer[pixel_idx]);
        const Color blendedColor = lerp(sourceColor, color, color.a);
        
        m_pixel_buffer[pixel_idx] = Color::to_rgba_u8(blendedColor);
    }

    // "Present" the canvas into a PNG image file.
    auto present() const -> bool;

    // Set whole canvas to the given pixel color.
    auto clear(const Color& clearColor = Color::kBlack) -> void;

    auto width() const { return m_dimensions.width; }
    auto height() const { return m_dimensions.height; }
    auto dimensions() const { return m_dimensions; }

    // No copy.
    Canvas(const Canvas& other) = delete;
    Canvas& operator=(const Canvas& other) = delete;

private:

    std::vector<RGBA_U8> m_pixel_buffer{};
    const Size2D m_dimensions{};
    const std::string m_name{};
};
