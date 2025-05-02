#include "lines.hpp"
#include <algorithm>

// Draws a line using Bresenham's algorithm, which only uses integer operations.
// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
auto draw_line(Canvas& canvas, const Point2D p0, const Point2D p1, const Color& color) -> void
{
    int dx = std::abs(p1.x - p0.x);
    int dy = std::abs(p1.y - p0.y);

    const bool is_steep = dy > dx;
    if (is_steep)
    {
        std::swap(dx, dy);
    }

    const int sx = (p0.x < p1.x) ? 1 : -1;
    const int sy = (p0.y < p1.y) ? 1 : -1;

    int err = (2 * dy) - dx;
    int x = p0.x;
    int y = p0.y;

    for (int i = 0; i <= dx; ++i)
    {
        canvas.draw_pixel({ x, y }, color);

        while (err > 0)
        {
            if (is_steep)
            {
                x += sx;
            }
            else
            {
                y += sy;
            }
            err -= 2 * dx;
        }

        if (is_steep)
        {
            y += sy;
        }
        else
        {
            x += sx;
        }
        err += 2 * dy;
    }
}
