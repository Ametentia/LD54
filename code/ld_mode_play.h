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

#define MAX_MUSIC_LAYERS 15

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

typedef u32 LD_ItemType;
enum LD_ItemType {
    LD_ITEM_TYPE_ROD = 0,
    LD_ITEM_TYPE_NET,
    LD_ITEM_TYPE_BOMBS,
    LD_ITEM_TYPE_TORCH,
    LD_ITEM_TYPE_LANTERN,
    LD_ITEM_TYPE_LILYPAD,
    LD_ITEM_TYPE_LADDER,

    LD_ITEM_TYPE_COUNT
};

typedef struct LD_ItemInfo LD_ItemInfo;
struct LD_ItemInfo {
    const char *name;

    LD_ShapeType base_shape; // unrotated
    LD_ItemType  type;
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

typedef u32 LD_MapBlockadeType;
enum LD_MapBlockadeType {
    LD_MAP_BLOCKADE_TYPE_NONE = 0,
    LD_MAP_BLOCKADE_TYPE_BUGS,
    LD_MAP_BLOCKADE_TYPE_ROCKS,
    LD_MAP_BLOCKADE_TYPE_UNK0, // torch counter :noblock
    LD_MAP_BLOCKADE_TYPE_UNK1, // lantern counter :noblock
    LD_MAP_BLOCKADE_TYPE_UNK2, // lilypad counter (stream?) :noblock
    LD_MAP_BLOCKADE_TYPE_WALL,
    LD_MAP_BLOCKADE_TYPE_POND
};

typedef struct LD_MapBlockade LD_MapBlockade;
struct LD_MapBlockade {
    const char *name;
    LD_MapBlockadeType type;
};

//
// @todo: instead of global, pre-initialise these with their image handles
//

global LD_MapBlockade blockades[LD_ITEM_TYPE_COUNT] = {
    { "pond",  LD_MAP_BLOCKADE_TYPE_POND  },
    { "bugs",  LD_MAP_BLOCKADE_TYPE_BUGS  },
    { "rocks", LD_MAP_BLOCKADE_TYPE_ROCKS },
    { "unk0",  LD_MAP_BLOCKADE_TYPE_NONE  },
    { "unk1",  LD_MAP_BLOCKADE_TYPE_NONE  },
    { "unk2",  LD_MAP_BLOCKADE_TYPE_NONE  },
    { "wall",  LD_MAP_BLOCKADE_TYPE_WALL  },
};

global LD_ItemInfo items[] = {
    { "rod",     LD_SHAPE_TYPE_Z,  LD_ITEM_TYPE_ROD      },
    { "net",     LD_SHAPE_TYPE_Z,  LD_ITEM_TYPE_NET      },
    { "bombs",   LD_SHAPE_TYPE_TU, LD_ITEM_TYPE_BOMBS    },
    { "torch",   LD_SHAPE_TYPE_I,  LD_ITEM_TYPE_TORCH    },
    { "lantern", LD_SHAPE_TYPE_O,  LD_ITEM_TYPE_LANTERN  },
    { "lilypad", LD_SHAPE_TYPE_L,  LD_ITEM_TYPE_LILYPAD  },
    { "ladder",  LD_SHAPE_TYPE_S,  LD_ITEM_TYPE_LADDER   }
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

    u32 occupancy;

    xiRandomState rng;

    struct {
        u32 total;
        u32 solution;

        LD_PlacedItem *first; // head of list
        LD_PlacedItem *last;  // tail of list, mainly to append to

        LD_PlacedItem *free;

        LD_PlacedItem *bag;
        LD_PlacedItem *picked;
    } items;

    u32 step_occupancy;
    u32 cindex;
    v4 *grid;

    bool bagOpen;
    bool itemSelected;
    LD_ShapePositions invShapes[7];
    LD_ShapePositions selected_shape;

    struct {
        b32 open;
        b32 hovered;
        f32 timer;

        u32 last_route;
        LD_MapRoute    *route;
        LD_MapBlockade *blockades;

        b32         recording;
        LD_MapRoute state;
    } map;

    u32 next_music_layer;
};

LD_MapRoute map_routes[] = {
    // { LD_MAP_DESTINATION_TOWN,      10, { { -3.326922f, -1.619220f }, { -2.857968f, -1.362622f }, { -2.539433f, -0.867124f }, { -2.583674f, -0.566285f }, { -2.813727f, -0.380473f }, { -2.875664f, -0.123875f }, { -2.645611f, 0.106178f }, { -2.672156f, 0.442410f }, { -2.530585f, 0.645919f }, { -2.627915f, 0.911364f } } },
    { LD_MAP_DESTINATION_TOWN,      4, { { -2.849120f, -1.353774f }, { -2.512888f, -0.867124f }, { -2.902209f, -0.141571f }, { -2.645611f, 0.433562f } } },
    { LD_MAP_DESTINATION_CASTLE,    5, { { -2.831423f, -1.362622f }, { -2.539433f, -0.822882f }, { -2.831423f, -0.398169f }, { -3.680850f, -0.238901f }, { -3.902055f, 0.442410f } } },
    { LD_MAP_DESTINATION_MOUNTAIN,  5, { { -2.857968f, -1.353774f }, { -2.574826f, -0.548588f }, { -2.663307f, 0.407017f }, { -1.911211f, 0.504347f }, { -1.247596f, 0.079634f }, } },
    { LD_MAP_DESTINATION_DRAGON,    6, { { -2.822575f, -1.344926f }, { -1.778488f, -0.539740f }, { -0.796338f, -0.884820f }, { 0.460106f, -1.247596f }, { 1.008695f, -1.451104f }, { 1.769639f, -1.566131f }, } },
    { LD_MAP_DESTINATION_GRAVEYARD, 6, { { -2.857968f, -1.362622f }, { -2.557129f, -0.849427f }, { -0.796338f, -0.849427f }, { 0.530892f, -0.743249f }, { 1.159114f, -0.433562f }, { 0.468955f, 0.778641f }, } },
    { LD_MAP_DESTINATION_VOLCANO,   7, { { -2.787182f, -1.353774f }, { -1.796184f, -0.548588f }, { -0.760945f, -0.893668f }, { 0.946757f, -0.575133f }, { 1.937755f, -0.176964f }, { 2.521736f, -1.229900f }, { 3.105717f, -1.035239f }, } },
    { LD_MAP_DESTINATION_TEMPLE,    8, { { -2.548281f, -0.831731f }, { -0.822882f, -0.840579f }, { 0.495499f, -1.238747f }, { 0.955606f, -0.575133f }, { 1.911211f, -0.203509f }, { 2.468647f, -0.088482f }, { 3.477342f, 0.362776f }, { 4.123260f, 1.167962f }, }, }
};

#endif  // LD_MODE_PLAY_H_
