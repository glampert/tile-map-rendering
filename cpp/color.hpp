#pragma once

#include "utils.hpp"

struct RGBA_U8
{
    std::uint8_t r{ 0 };
    std::uint8_t g{ 0 };
    std::uint8_t b{ 0 };
    std::uint8_t a{ 0 };
};

struct Color
{
    float r{ 0.0f };
    float g{ 0.0f };
    float b{ 0.0f };
    float a{ 0.0f };

    static const Color kBlack;
    static const Color kWhite;
    static const Color kGray;
    static const Color kBrightRed;
    static const Color kBrightGreen;
    static const Color kBrightBlue;
    static const Color kBrightYellow;
    static const Color kDarkRed;
    static const Color kDarkGreen;
    static const Color kDarkBlue;
    static const Color kDarkYellow;
    static const Color kCyan;
    static const Color kPurple;

    static auto to_rgba_u8(const Color& c) -> RGBA_U8
    {
        assert(is_normalized(c.r) &&
               is_normalized(c.g) &&
               is_normalized(c.b) &&
               is_normalized(c.a));
        
        return {
            .r = static_cast<std::uint8_t>(c.r * 255.0f),
            .g = static_cast<std::uint8_t>(c.g * 255.0f),
            .b = static_cast<std::uint8_t>(c.b * 255.0f),
            .a = static_cast<std::uint8_t>(c.a * 255.0f) };
    }

    static auto from_rgba_u8(const RGBA_U8 u) -> Color
    {
        return {
            .r = static_cast<float>(u.r) / 255.0f,
            .g = static_cast<float>(u.g) / 255.0f,
            .b = static_cast<float>(u.b) / 255.0f,
            .a = static_cast<float>(u.a) / 255.0f };
    }
};

inline auto operator*(const Color& lhs, const Color& rhs) -> Color
{
    return {
        lhs.r * rhs.r,
        lhs.g * rhs.g,
        lhs.b * rhs.b,
        lhs.a * rhs.a };
}

inline auto operator*(const Color& c, const float amount) -> Color
{
    return {
        c.r * amount,
        c.g * amount,
        c.b * amount,
        c.a * amount };
}

inline auto operator/(const Color& c, const float amount) -> Color
{
    return {
        c.r / amount,
        c.g / amount,
        c.b / amount,
        c.a / amount };
}

inline auto operator+(const Color& lhs, const Color& rhs) -> Color
{
    return {
        lhs.r + rhs.r,
        lhs.g + rhs.g,
        lhs.b + rhs.b,
        lhs.a + rhs.a };
}

inline auto operator-(const Color& lhs, const Color& rhs) -> Color
{
    return {
        lhs.r - rhs.r,
        lhs.g - rhs.g,
        lhs.b - rhs.b,
        lhs.a - rhs.a };
}

inline auto operator*=(Color& lhs, const Color& rhs) -> Color&
{
    lhs = lhs * rhs;
    return lhs;
}

inline auto operator*=(Color& c, const float amount) -> Color&
{
    c = c * amount;
    return c;
}

inline auto operator/=(Color& c, const float amount) -> Color&
{
    c = c / amount;
    return c;
}

inline auto operator+=(Color& lhs, const Color& rhs) -> Color&
{
    lhs = lhs + rhs;
    return lhs;
}

inline auto operator-=(Color& lhs, const Color& rhs) -> Color&
{
    lhs = lhs - rhs;
    return lhs;
}

inline auto is_normalized(const Color& c)
{
    return is_normalized(c.r) &&
           is_normalized(c.g) &&
           is_normalized(c.b) &&
           is_normalized(c.a);
}

inline auto is_zero(const Color& c)
{
    return (c.r == 0.0f && c.g == 0.0f && c.b == 0.0f && c.a == 0.0f);
}

inline auto lerp(const Color& a, const Color& b, const float t) -> Color
{
    return {
        .r = lerp(a.r, b.r, t),
        .g = lerp(a.g, b.g, t),
        .b = lerp(a.b, b.b, t),
        .a = lerp(a.a, b.a, t) };
}
