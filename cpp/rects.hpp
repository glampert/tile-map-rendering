#pragma once

#include "canvas.hpp"
#include "texture.hpp"

auto draw_wireframe_rect(Canvas& canvas,
                         const Point2D top_left,
                         const Point2D bottom_left,
                         const Point2D top_right,
                         const Point2D bottom_right,
                         const Color& color) -> void;

auto draw_filled_rect(Canvas& canvas,
                      const Point2D top_left,
                      const Point2D bottom_left,
                      const Point2D top_right,
                      const Point2D bottom_right,
                      const Color& color) -> void;

auto draw_shaded_rect(Canvas& canvas,
                      const Point2D top_left,
                      const float top_left_intensity,
                      const Point2D bottom_left,
                      const float bottom_left_intensity,
                      const Point2D top_right,
                      const float top_right_intensity,
                      const Point2D bottom_right,
                      const float bottom_right_intensity,
                      const Color& color) -> void;

auto draw_textured_rect(Canvas& canvas,
                        const Point2D top_left,
                        const TexCoords top_left_tc,
                        const Point2D bottom_left,
                        const TexCoords bottom_left_tc,
                        const Point2D top_right,
                        const TexCoords top_right_tc,
                        const Point2D bottom_right,
                        const TexCoords bottom_right_tc,
                        const Texture& texture,
                        const Color& color) -> void;
