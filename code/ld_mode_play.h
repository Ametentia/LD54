#if !defined(LD_MODE_PLAY_H_)
#define LD_MODE_PLAY_H_

#define PI 3.1415926535897932384626f

// various gameplay and visual related constants
//
#define GRID_WIDTH  6
#define GRID_HEIGHT 5

#define GRID_TILE_DIM  0.5f
#define GRID_BASE_X    1.65f
#define GRID_BASE_Y   -0.8f

global v4 debug_colours[] =
{
    /* 00 */ { 1, 0, 0, 1},
    /* 01 */ { 0, 1, 0, 1},
    /* 02 */ { 0, 0, 1, 1},
    /* 03 */ { 1, 1, 0, 1},
    /* 04 */ { 0, 1, 1, 1},
    /* 05 */ { 1, 0, 1, 1},
    /* 06 */ { 1, 0.5f, 0, 1},
    /* 07 */ { 1, 0, 0.5f, 1},
    /* 08 */ { 0.5f, 1, 0, 1},
    /* 09 */ { 0, 1, 0.5f, 1},
    /* 10 */ { 0.5f, 0, 1, 1},
    /* 11 */ { 1, 0.75f, 0.5f, 1},
    /* 12 */ { 1, 0.5f, 0.75f, 1},
    /* 13 */ { 0.75f, 1, 0.5f, 1},
    /* 14 */ { 0.5f, 1, 0.75f, 1},
    /* 15 */ { 0.5f, 0.75f, 1, 1},

    /* 16 */ {1, 0.25f, 0.25f, 1},
    /* 17 */ {0.25f, 1, 0.25f, 1},
    /* 18 */ {0.25f, 0.25f, 1, 1},
    /* 19 */ {1, 1, 0.25f, 1},
    /* 20 */ {0.25f, 1, 1, 1},
    /* 21 */ {1, 0.25f, 1, 1},
    /* 22 */ {1, 0.5f, 0.25f, 1},
    /* 23 */ {1, 0.25f, 0.5f, 1},
    /* 24 */ {0.5f, 1, 0.25f, 1},
    /* 25 */ {0.25f, 1, 0.5f, 1},
    /* 26 */ {0.5f, 0.25f, 1, 1},
    /* 27 */ {1, 0.25f, 0.5f, 1},
    /* 28 */ {1, 0.5f, 0.25f, 1},
    /* 29 */ {0.25f, 1, 0.5f, 1},
    /* 30 */ {0.5f, 1, 0.25f, 1},
    /* 31 */ {0.5f, 0.25f, 1, 1}
};

typedef u32 LD_ShapeType;
enum LD_ShapeType {
    LD_SHAPE_TYPE_I,
    LD_SHAPE_TYPE_IR,

    LD_SHAPE_TYPE_L,
    LD_SHAPE_TYPE_J,
    LD_SHAPE_TYPE_LR,
    LD_SHAPE_TYPE_JR,

    LD_SHAPE_TYPE_O,
    LD_SHAPE_TYPE_OR,

    LD_SHAPE_TYPE_T,
    LD_SHAPE_TYPE_TR,
    LD_SHAPE_TYPE_TU,
    LD_SHAPE_TYPE_TL,

    LD_SHAPE_TYPE_S,
    LD_SHAPE_TYPE_SR,

    LD_SHAPE_TYPE_Z,
    LD_SHAPE_TYPE_ZR,

    LD_SHAPE_TYPE_COUNT
};

typedef struct LD_ShapeInfo LD_ShapeInfo;
struct LD_ShapeInfo {
    u32 offset_count;
    v2s offsets[4];
};
typedef struct LD_ShapePositions LD_ShapePositions;
struct LD_ShapePositions {
    u32 quad_number;
    v2 positions[5];
};

function u32 LD_CoordToOccupancy(u32 x, u32 y) {
    u32 result = 0;
    if (x < GRID_WIDTH && y < GRID_HEIGHT) {
        result = 1 << ((y * GRID_WIDTH) + x);
    }

    return result;
}


global LD_ShapeInfo shapes[LD_SHAPE_TYPE_COUNT] = {
    2, { {  0, -1 }, {  0,  1 }            }, // I
    2, { { -1,  0 }, {  1,  0 }            }, // IR

    2, { {  1,  0 }, {  0,  1 }            }, // L
    2, { { -1,  0 }, {  0,  1 }            }, // J
    2, { { -1,  0 }, {  0, -1 }            }, // LR
    2, { {  0, -1 }, {  1,  0 }            }, // JR

    1, { {  1,  0 }                        }, // O
    1, { {  0,  1 }                        }, // OR

    3, { {  1,  0 }, { -1,  0 }, {  0, -1 } }, // T
    3, { {  0,  1 }, {  1,  0 }, {  0, -1 } }, // TR
    3, { {  1,  0 }, { -1,  0 }, {  0,  1 } }, // TU
    3, { {  0,  1 }, { -1,  0 }, {  0, -1 } }, // TL

    3, { { -1,  0 }, {  0,  1 }, {  1,  1 } }, // S
    3, { {  0, -1 }, { -1,  0 }, { -1,  1 } }, // SR
    3, { { -1,  0 }, {  0, -1 }, {  1, -1 } }, // Z
    3, { {  0, -1 }, {  1,  0 }, {  1,  1 } }, // ZR
};

typedef u32 LD_ItemCategory;
enum LD_ItemCategory {
    LD_ITEM_CATEGORY_DESTRUCTIVE = (1 << 0),
    LD_ITEM_CATEGORY_TRAVERSAL   = (1 << 1),
    LD_ITEM_CATEGORY_COMBATIVE   = (1 << 2),
    LD_ITEM_CATEGORY_DEFENSIVE   = (1 << 3),
    LD_ITEM_CATEGORY_COOLING     = (1 << 4),
    LD_ITEM_CATEGORY_HEATING     = (1 << 5),
    LD_ITEM_CATEGORY_ENCOUNTER   = (1 << 6),
    LD_ITEM_CATEGORY_HEALING     = (1 << 7),
    LD_ITEM_CATEGORY_FOOD        = (1 << 8),
};

typedef struct LD_ItemInfo LD_ItemInfo;
struct LD_ItemInfo {
    const char *name;

    LD_ShapeType    base_shape; // unrotated
    LD_ItemCategory categories;
};

typedef struct LD_PlacedItem LD_PlacedItem;
struct LD_PlacedItem {
    b32 placed;
    u32 occupancy;

    f32 pick_x, pick_y; // position _before_ the item was picked
    f32 x, y;
    u32 layer;
    LD_ShapeType shape; // with rotation

    LD_ShapeType rotation_min;
    LD_ShapeType rotation_max;

    LD_ItemInfo   *info;

    LD_PlacedItem *next;
    LD_PlacedItem *next_bag; // next in bag
    LD_PlacedItem *prev_bag; // prev in bag
};

global LD_ItemInfo items[] = {
    { "rod",     LD_SHAPE_TYPE_Z,  0 },
    { "net",     LD_SHAPE_TYPE_Z,  0 },
    { "bombs",   LD_SHAPE_TYPE_TU, 0 },
    { "torch",   LD_SHAPE_TYPE_I,  0 },
    { "lantern", LD_SHAPE_TYPE_O,  0 },
    { "lilypad", LD_SHAPE_TYPE_L,  0 },
    { "ladder",  LD_SHAPE_TYPE_S,  0 }
};

typedef u32 LD_MapDestination;
enum LD_MapDestination {
    LD_MAP_DESTINATION_CASTLE,
    LD_MAP_DESTINATION_TOWN,
    LD_MAP_DESTINATION_MOUNTAIN,
    LD_MAP_DESTINATION_DRAGON,
    LD_MAP_DESTINATION_GRAVEYARD,
    LD_MAP_DESTINATION_VOLCANO,
    LD_MAP_DESTINATION_TEMPLE
};

typedef struct LD_MapRoute LD_MapRoute;
struct LD_MapRoute {
    LD_MapDestination dest;

    u32 node_count;
    v2  nodes[12];
};

struct LD_ModePlay {
    LD_Context *ld;

    xiCameraTransform camera;

    xiArena *arena;
    xiLogger logger;

    xiAnimation hero;
    xiAnimation heroFlame;

    u32 occupancy;

    xiRandomState rng;

    LD_PlacedItem *bag_items;
    LD_PlacedItem *placed_items;

    LD_PlacedItem *picked;

    LD_PlacedItem *free_items;

    u32 step_occupancy;
    u32 cindex;
    v4 *grid;

    bool bagOpen;
    bool itemSelected;
    LD_ShapePositions invShapes[7];
    LD_ShapePositions selected_shape;

    b32 map_open;

    b32 map_hovered;
    f32 map_timer;

    f32 puzzle_timer;

    b32 recording;
    LD_MapRoute  route;
    LD_MapRoute *selected;
};

LD_MapRoute map_routes[] = {
    { LD_MAP_DESTINATION_TOWN,      4, { { -2.849120f, -1.353774f }, { -2.512888f, -0.867124f }, { -2.902209f, -0.141571f }, { -2.645611f, 0.433562f } } },
    { LD_MAP_DESTINATION_CASTLE,    5, { { -2.831423f, -1.362622f }, { -2.539433f, -0.822882f }, { -2.831423f, -0.398169f }, { -3.680850f, -0.238901f }, { -3.902055f, 0.442410f } } },
    { LD_MAP_DESTINATION_MOUNTAIN,  5, { { -2.857968f, -1.353774f }, { -2.574826f, -0.548588f }, { -2.663307f, 0.407017f }, { -1.911211f, 0.504347f }, { -1.247596f, 0.079634f }, } },
    { LD_MAP_DESTINATION_DRAGON,    6, { { -2.822575f, -1.344926f }, { -1.778488f, -0.539740f }, { -0.796338f, -0.884820f }, { 0.460106f, -1.247596f }, { 1.008695f, -1.451104f }, { 1.769639f, -1.566131f }, } },
    { LD_MAP_DESTINATION_GRAVEYARD, 6, { { -2.857968f, -1.362622f }, { -2.557129f, -0.849427f }, { -0.796338f, -0.849427f }, { 0.530892f, -0.743249f }, { 1.159114f, -0.433562f }, { 0.468955f, 0.778641f }, } },
    { LD_MAP_DESTINATION_VOLCANO,   7, { { -2.787182f, -1.353774f }, { -1.796184f, -0.548588f }, { -0.760945f, -0.893668f }, { 0.946757f, -0.575133f }, { 1.937755f, -0.176964f }, { 2.521736f, -1.229900f }, { 3.105717f, -1.035239f }, } },
    { LD_MAP_DESTINATION_TEMPLE,    8, { { -2.548281f, -0.831731f }, { -0.822882f, -0.840579f }, { 0.495499f, -1.238747f }, { 0.955606f, -0.575133f }, { 1.911211f, -0.203509f }, { 2.468647f, -0.088482f }, { 3.477342f, 0.362776f }, { 4.123260f, 1.167962f }, }, }
};

function void LD_MusicInit(LD_ModePlay *play);

#endif  // LD_MODE_PLAY_H_
