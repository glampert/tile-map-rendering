#pragma once

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cassert>

// 2D Cartesian coordinates point.
struct Point2D
{
    int x{ 0 };
    int y{ 0 };
};

// 2D dimensions (width & height).
struct Size2D
{
    int width{ 0 };
    int height{ 0 };
    
    auto is_valid() const
    {
        return width > 0 && height > 0;
    }
};

inline auto is_zero(const float f)
{
    return (f == 0.0f);
}

inline auto is_normalized(const float f)
{
    return (f >= 0.0f && f <= 1.0f);
}

inline auto lerp(const float a, const float b, const float t)
{
    return (1.0f - t) * a + t * b;
}

inline auto frac(const float x)
{
    return x - std::floor(x);
}
