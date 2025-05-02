#include "texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#define STBI_ONLY_PNG
#include "external/stb_image.h"

#include <print>

auto Texture::load_from_file(const std::string& filename, const Filter filter) -> bool
{
    stbi_set_flip_vertically_on_load(true);
    
    int img_w = 0, img_h = 0, img_channels_in_file = 0;
    constexpr int img_desired_channels = 4; // RGBA_U8

    auto* const img_data = stbi_load(filename.c_str(), &img_w, &img_h, &img_channels_in_file, img_desired_channels);
    if (img_data == nullptr)
    {
        std::println(stderr, "Error: Failed to load image: '{}'", filename);
        return false;
    }

    assert(img_w != 0);
    assert(img_h != 0);
    
    m_dimensions.width = img_w;
    m_dimensions.height = img_h;
    m_filter = filter;

    const auto img_num_pixels = static_cast<std::size_t>(img_w * img_h);
    m_pixels.reserve(img_num_pixels);

    auto* img_cursor = img_data;
    for (std::size_t p = 0; p < img_num_pixels; p++, img_cursor += 4)
    {
        m_pixels.push_back(RGBA_U8{
            .r = img_cursor[0],
            .g = img_cursor[1],
            .b = img_cursor[2],
            .a = img_cursor[3],
        });
    }
    
    stbi_image_free(img_data);
    return true;
}

auto Texture::sample_texel(const TexCoords tex_coords) const -> Color
{
    float tx = std::clamp(tex_coords.u, 0.0f, 1.0f);
    float ty = std::clamp(tex_coords.v, 0.0f, 1.0f);

    switch (m_filter)
    {
    case Filter::kNearest:
        {
            // Nearest neighbor (point) filtering.
            const auto x = static_cast<int>(tx * m_dimensions.width);
            const auto y = static_cast<int>(ty * m_dimensions.height);
            return pixel_at(x, y);
        }
    case Filter::kBilinear:
        {
            const float fx = frac(tx);
            const float fy = frac(ty);

            tx = std::floor(tx * m_dimensions.width);
            ty = std::floor(ty * m_dimensions.height);

            const auto x = static_cast<int>(tx);
            const auto y = static_cast<int>(ty);

            const Color top_left = pixel_at(x, y);
            const Color top_right = pixel_at(x + 1, y);
            const Color bottom_left = pixel_at(x, y + 1);
            const Color bottom_right = pixel_at(x + 1, y + 1);

            // Linearly interpolate the neighboring pixels.
            const Color color_top = lerp(top_right, top_left, fx);
            const Color color_bottom = lerp(bottom_right, bottom_left, fx);

            return lerp(color_bottom, color_top, fy);
        }
    }
}
