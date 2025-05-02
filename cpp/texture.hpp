#pragma once

#include "utils.hpp"
#include "color.hpp"

#include <string>
#include <vector>

struct TexCoords
{
    float u{ 0.0f };
    float v{ 0.0f };
};

inline auto is_normalized(const TexCoords tc)
{
    return is_normalized(tc.u) && is_normalized(tc.v);
}

inline auto lerp(const TexCoords a, const TexCoords b, const float t) -> TexCoords
{
    return {
        .u = lerp(a.u, b.u, t),
        .v = lerp(a.v, b.v, t) };
}

class Texture final
{
public:
    
    enum class Filter : int
    {
        kNearest,
        kBilinear
    };

    Texture() = default;

    Texture(const std::string& filename, const Filter filter)
    {
        [[maybe_unused]] const bool is_loaded = load_from_file(filename, filter);
        assert(is_loaded);
    }
    
    auto load_from_file(const std::string& filename, const Filter filter) -> bool;

    // Get color for a texel with filtering applied.
    auto sample_texel(const TexCoords tex_coords) const -> Color;

    // Sample pixel directly without applying any filtering.
    auto pixel_at(int x, int y) const -> Color
    {
        x = std::min(x, m_dimensions.width  - 1);
        y = std::min(y, m_dimensions.height - 1);

        const auto offset = static_cast<std::size_t>(x + (y * m_dimensions.width));
        assert(offset < m_pixels.size());

        const auto px = m_pixels[offset];
        return Color::from_rgba_u8(px);
    }

    auto is_valid() const { return !m_pixels.empty() && m_dimensions.is_valid(); }
    auto width() const { return m_dimensions.width; }
    auto height() const { return m_dimensions.height; }
    auto dimensions() const { return m_dimensions; }
    auto filter() const { return m_filter; }
    
    // No copy.
    Texture(const Texture& other) = delete;
    Texture& operator=(const Texture& other) = delete;

private:
    
    Size2D m_dimensions{};
    Filter m_filter{};
    std::vector<RGBA_U8> m_pixels{};
};
