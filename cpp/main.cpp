#include <print>
#include <array>
#include <algorithm>

#include "canvas.hpp"
#include "lines.hpp"
#include "rects.hpp"

#define USE_ISO_COORDS 1
#define DRAW_DEBUG_BOUNDS 0
#define MIRROR_TILE_TEXCOORDS 0

constexpr int kBaseTileWidth  = 64;
constexpr int kBaseTileHeight = 32;

constexpr int kHalfBaseTileWidth  = (kBaseTileWidth  / 2);
constexpr int kHalfBaseTileHeight = (kBaseTileHeight / 2);

// 2D point in isometric space.
struct IsoPoint2D
{
    int x{ 0 };
    int y{ 0 };
};

inline auto isometric_to_cartesian(const IsoPoint2D iso) -> Point2D
{
    const int x = ((iso.x / kHalfBaseTileWidth)  + (iso.y / kHalfBaseTileHeight)) / 2;
    const int y = ((iso.y / kHalfBaseTileHeight) - (iso.x / kHalfBaseTileWidth))  / 2;
    return { x, y };
}

inline auto cartesian_to_isometric(const Point2D pt) -> IsoPoint2D
{
    const int iso_x = (pt.x - pt.y) * kHalfBaseTileWidth;
    const int iso_y = (pt.x + pt.y) * kHalfBaseTileHeight;
    return { iso_x, iso_y };
}

struct TileDef
{
    std::string name{};
    Size2D logical_size{}; // Logical size for the tile map. Always a multiple of the base tile size.
    Size2D draw_size{};    // Draw size for tile rendering. Can be any size ratio.
    const Texture* texture{};
    Color tint{ Color::kWhite };
};

struct TileDrawParams
{
    const TileDef& def;
    IsoPoint2D pos{};
    Size2D size{}; // Draw size.
    Color tint{ Color::kWhite };
    bool debug_draw_bounds{ false };
};

auto draw_tile(Canvas& canvas, const TileDrawParams& params)
{
    assert(params.size.is_valid());
    assert(params.def.texture != nullptr);

    const int scaling = 2;
    const int offset_x = USE_ISO_COORDS ? 448 : 0;
    const int offset_y = USE_ISO_COORDS ? 128 : 0;
    
    const auto tile_width  = (params.size.width  * scaling);
    const auto tile_height = (params.size.height * scaling);
    
    const auto tile_pos_x = (params.pos.x * scaling) + offset_x;
    const auto tile_pos_y = (params.pos.y * scaling) + offset_y;

    const Point2D top_left     = { tile_pos_x,  tile_pos_y + tile_height };
    const Point2D bottom_left  = { tile_pos_x,  tile_pos_y };
    const Point2D top_right    = { tile_pos_x + tile_width, tile_pos_y + tile_height };
    const Point2D bottom_right = { tile_pos_x + tile_width, tile_pos_y };

#if MIRROR_TILE_TEXCOORDS
    constexpr TexCoords top_left_tc     = { 1.0f, 1.0f };
    constexpr TexCoords bottom_left_tc  = { 1.0f, 0.0f };
    constexpr TexCoords top_right_tc    = { 0.0f, 1.0f };
    constexpr TexCoords bottom_right_tc = { 0.0f, 0.0f };
#else
    constexpr TexCoords top_left_tc     = { 0.0f, 1.0f };
    constexpr TexCoords bottom_left_tc  = { 0.0f, 0.0f };
    constexpr TexCoords top_right_tc    = { 1.0f, 1.0f };
    constexpr TexCoords bottom_right_tc = { 1.0f, 0.0f };
#endif
    
    draw_textured_rect(canvas,
                       top_left,     top_left_tc,
                       bottom_left,  bottom_left_tc,
                       top_right,    top_right_tc,
                       bottom_right, bottom_right_tc,
                       *params.def.texture,
                       params.def.tint * params.tint);

    if (params.debug_draw_bounds)
    {
        draw_wireframe_rect(canvas, top_left, bottom_left, top_right, bottom_right, Color::kDarkRed);
    }
}

auto draw_terrain(Canvas& canvas, const TileDef& tile_def, const Point2D tile_pos)
{
    // Terrain tiles size is constrained.
    assert(tile_def.logical_size.width  == kBaseTileWidth);
    assert(tile_def.logical_size.height == kBaseTileHeight);

#if USE_ISO_COORDS
    const IsoPoint2D tile_coords = cartesian_to_isometric(tile_pos);
#else
    const IsoPoint2D tile_coords = { .x = tile_pos.x * kBaseTileWidth, .y = tile_pos.y * kBaseTileHeight };
#endif

    draw_tile(canvas, {
        .def  = tile_def,
        .pos  = tile_coords,
        .size = tile_def.draw_size
    });
}

auto draw_building(Canvas& canvas, const TileDef& tile_def, const Point2D tile_pos)
{
#if USE_ISO_COORDS
    // Convert the base tile into isometric screen coordinates:
    IsoPoint2D tile_coords = cartesian_to_isometric(tile_pos);

    // Adjust to center the building image:
    tile_coords.x += (kBaseTileWidth / 2) - (tile_def.logical_size.width / 2);
#else
    const IsoPoint2D tile_coords = { .x = tile_pos.x * kBaseTileWidth, .y = tile_pos.y * kBaseTileHeight };
#endif

    draw_tile(canvas, {
        .def  = tile_def,
        .pos  = tile_coords,
        .size = tile_def.draw_size,
        .debug_draw_bounds = DRAW_DEBUG_BOUNDS
    });
}

auto draw_unit(Canvas& canvas, const TileDef& tile_def, const Point2D tile_pos)
{
#if USE_ISO_COORDS
    // Convert the base tile into isometric screen coordinates:
    IsoPoint2D tile_coords = cartesian_to_isometric(tile_pos);

    // Adjust to center the unit sprite:
    tile_coords.x += (kBaseTileWidth / 2) - (tile_def.draw_size.width / 2);
    tile_coords.y += (tile_def.draw_size.height / 2);
#else
    const IsoPoint2D tile_coords = { .x = tile_pos.x * kBaseTileWidth, .y = tile_pos.y * kBaseTileHeight };
#endif

    draw_tile(canvas, {
        .def  = tile_def,
        .pos  = tile_coords,
        .size = tile_def.draw_size,
        .debug_draw_bounds = DRAW_DEBUG_BOUNDS
    });
}

auto main([[maybe_unused]] const int argc,
          [[maybe_unused]] const char* argv[]) -> int
{
    std::println("Tile Map Render");
    
    Canvas canvas{
        Size2D{ 1024, 768 },
        "tile_map",
        Color::kWhite
    };

    const Texture tex_ground_0{ "assets/tile_64x32_0.png",    Texture::Filter::kBilinear };
    const Texture tex_ground_1{ "assets/tile_64x32_1.png",    Texture::Filter::kBilinear };
    const Texture tex_house   { "assets/tile_256x136.png",    Texture::Filter::kBilinear };
    const Texture tex_building{ "assets/tile_256x196.png",    Texture::Filter::kNearest  };
    const Texture tex_ped     { "assets/tile_unit_32x42.png", Texture::Filter::kNearest  };

    const TileDef tile_defs[] = {
        { .name = "ground_0", .logical_size = { 64,  32 }, .draw_size = { 64,  32  }, .texture = &tex_ground_0, .tint = Color::kDarkYellow },
        { .name = "ground_1", .logical_size = { 64,  32 }, .draw_size = { 64,  32  }, .texture = &tex_ground_1 },
        { .name = "house",    .logical_size = { 128, 64 }, .draw_size = { 128, 68  }, .texture = &tex_house },
        { .name = "building", .logical_size = { 192, 96 }, .draw_size = { 192, 144 }, .texture = &tex_building },
        { .name = "ped",      .logical_size = { 64,  32 }, .draw_size = { 16,  24  }, .texture = &tex_ped },
    };

    constexpr int kMapHeight = 8;
    constexpr int kMapWidth  = 8;

    constexpr int X = 0; // ground/empty
    constexpr int R = 1; // road
    constexpr int H = 2; // house (2x2)
    constexpr int B = 3; // building (3x3)
    constexpr int U = 4; // unit (ped)

    const std::array<int, kMapHeight * kMapWidth> terrain_layer_map = {
        R,R,R,R,R,R,R,R, // <-- start, tile zero is the leftmost
        R,X,X,X,X,X,X,R,
        R,X,X,X,X,X,X,R,
        R,X,X,X,X,X,X,R,
        R,X,X,X,X,X,X,R,
        R,X,X,X,X,X,X,R,
        R,X,X,X,X,X,X,R,
        R,R,R,R,R,R,R,R,
    };

    const std::array<int, kMapHeight * kMapWidth> buildings_and_units_layer_map = {
        U,U,U,U,U,U,U,U, // <-- start, tile zero is the leftmost
        U,B,X,X,U,H,X,U,
        U,X,X,X,U,X,X,U,
        U,X,X,X,U,H,X,U,
        U,U,U,U,U,X,X,U,
        U,H,X,U,U,H,X,U,
        U,X,X,U,U,X,X,U,
        U,U,U,U,U,U,U,U,
    };

    // Base layer (terrain):
    for (int y = kMapHeight - 1; y >= 0; --y)
    {
        for (int x = kMapWidth - 1; x >= 0; --x)
        {
            const auto map_idx = static_cast<std::size_t>(x + (y * kMapWidth));
            const auto tile_idx = terrain_layer_map[map_idx];

            const TileDef& tile_def = tile_defs[tile_idx];
            draw_terrain(canvas, tile_def, { x, y });
        }
    }

    struct DrawTileCmd
    {
        const TileDef* tile_def;
        Point2D tile_pos;
        int tile_idx;
        int y_sort; // Y value of the bottom left corner of the tile image, for sorting.
    };
    std::vector<DrawTileCmd> buildings_and_units_draw_list{};

    // Buildings & units layer:
    for (int y = kMapHeight - 1; y >= 0; --y)
    {
        for (int x = kMapWidth - 1; x >= 0; --x)
        {
            const auto map_idx = static_cast<std::size_t>(x + (y * kMapWidth));
            const auto tile_idx = buildings_and_units_layer_map[map_idx];

            if (tile_idx != X)
            {
                const Point2D tile_pos = { x, y };
                const TileDef& tile_def = tile_defs[tile_idx];
                const int y_sort = cartesian_to_isometric(tile_pos).y + tile_def.logical_size.height;
                
                buildings_and_units_draw_list.push_back({
                    .tile_def = &tile_def,
                    .tile_pos = tile_pos,
                    .tile_idx = tile_idx,
                    .y_sort   = y_sort
                });
            }
        }
    }
 
    std::stable_sort(buildings_and_units_draw_list.begin(), buildings_and_units_draw_list.end(),
        [](const auto& a, const auto& b)
        {
            return a.y_sort > b.y_sort;
        });

    for (const auto& cmd : buildings_and_units_draw_list)
    {
        if (cmd.tile_idx == U)
        {
            draw_unit(canvas, *cmd.tile_def, cmd.tile_pos);
        }
        else
        {
            draw_building(canvas, *cmd.tile_def, cmd.tile_pos);
        }
    }
    
    [[maybe_unused]] const bool result = canvas.present();
    assert(result == true);
}
