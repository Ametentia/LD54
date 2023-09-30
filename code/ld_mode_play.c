#define GRID_WIDTH  6
#define GRID_HEIGHT 5

#define PI 3.1415926535897932384626f

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

function u32 LD_CoordToOccupancy(u32 x, u32 y) {
    u32 result = 0;
    if (x < GRID_WIDTH && y < GRID_HEIGHT) {
        result = 1 << ((y * GRID_WIDTH) + x);
    }

    return result;
}

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

    f32 x, y;
    u32 layer;
    LD_ShapeType shape; // with rotation

    LD_ItemInfo   *info;

    LD_PlacedItem *next;
    LD_PlacedItem *next_bag; // next in bag
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

struct LD_ModePlay {
    LD_Context *ld;

    xiCameraTransform camera;

    xiArena *arena;
    xiLogger logger;

    u32 occupancy;

    xiRandomState rng;

    LD_ShapeType min;  // base type
    LD_ShapeType max;  // max for current type
    LD_ShapeType type; // current type including rotation

    LD_PlacedItem *bag_items;
    LD_PlacedItem *placed_items;

    LD_PlacedItem *free_items;

    u32 step_occupancy;
    u32 cindex;
    v4 *grid;
};

function LD_PlacedItem *LD_PlacedItemGet(LD_ModePlay *play) {
    LD_PlacedItem *result = play->free_items;
    if (!result) { result = xi_arena_push_array(play->arena, LD_PlacedItem, 1); }
    else { play->free_items = result->next; }

    result->placed    = false;
    result->occupancy = 0;

    result->x = result->y = 0;
    result->shape = 0;

    result->info = 0;
    result->next = 0;

    return result;
}

function void LD_PlacedItemRemove(LD_ModePlay *play, LD_PlacedItem *item) {
    item->next = play->free_items;
    play->free_items = item;
}

function LD_ShapeType LD_ShapeTypeToBaseType(LD_ShapeType type) {
    LD_ShapeType result;
    switch (type) {
        case LD_SHAPE_TYPE_IR: { result = LD_SHAPE_TYPE_I; } break;

        case LD_SHAPE_TYPE_J:
        case LD_SHAPE_TYPE_LR:
        case LD_SHAPE_TYPE_JR: { result = LD_SHAPE_TYPE_L; } break;

        case LD_SHAPE_TYPE_OR: { result = LD_SHAPE_TYPE_O; } break;

        case LD_SHAPE_TYPE_T:
        case LD_SHAPE_TYPE_TR:
        case LD_SHAPE_TYPE_TL: { result = LD_SHAPE_TYPE_TU; } break;

        case LD_SHAPE_TYPE_SR: { result = LD_SHAPE_TYPE_S;  } break;
        case LD_SHAPE_TYPE_ZR: { result = LD_SHAPE_TYPE_Z;  } break;

        default: { result = type; } break;
    }

    return result;
}

function u32 LD_PlaceShape(LD_ModePlay *play, u32 occupancy, LD_ShapeType type, u32 x, u32 y) {
    LD_ShapeInfo *info = &shapes[type];

    rect3 bounds = xi_camera_bounds_get(&play->camera);

    u32 result = LD_CoordToOccupancy(x, y);
    for (u32 it = 0; it < info->offset_count; ++it) {
        result |= LD_CoordToOccupancy(x + info->offsets[it].x, y + info->offsets[it].y);
    }

    if (__popcnt(result) == (info->offset_count + 1) && ((occupancy & result) == 0)) {
        play->grid[(y * GRID_WIDTH) + x] = debug_colours[play->cindex];
        for (u32 it = 0; it < info->offset_count; ++it) {
            s32 lx = x + info->offsets[it].x;
            s32 ly = y + info->offsets[it].y;

            play->grid[(ly * GRID_WIDTH) + lx] = debug_colours[play->cindex];
        }

        LD_PlacedItem *item = LD_PlacedItemGet(play);

        // used to lookup an item type
        //
        LD_ShapeType base = LD_ShapeTypeToBaseType(type);

        item->x     = xi_rng_range_f32(&play->rng, bounds.min.x, 0.5f * (bounds.max.x + bounds.min.x));
        item->y     = xi_rng_range_f32(&play->rng, bounds.min.y, (bounds.max.y - bounds.min.y));
        item->shape = base;
        item->next  = play->placed_items;

        for (u32 it = 0; it < XI_ARRAY_SIZE(items); ++it) {
            if (items[it].base_shape == base) {
                item->info = &items[it];
                break;
            }
        }

        XI_ASSERT(item->info != 0);

        play->placed_items = item;

        play->cindex += 1;
        play->cindex %= XI_ARRAY_SIZE(debug_colours);
    }
    else {
        result = 0;
    }

    return result;
}

function void LD_SolutionClear(LD_ModePlay *play) {
    play->cindex    = 0;
    play->occupancy = 0;

    play->occupancy |= LD_CoordToOccupancy(0,              GRID_HEIGHT - 1);
    play->occupancy |= LD_CoordToOccupancy(GRID_WIDTH - 1, GRID_HEIGHT - 1);

    // ~debug
    //
    play->step_occupancy = 0;

    play->step_occupancy |= LD_CoordToOccupancy(0,              GRID_HEIGHT - 1);
    play->step_occupancy |= LD_CoordToOccupancy(GRID_WIDTH - 1, GRID_HEIGHT - 1);

    for (u32 it = 0; it < 32; ++it) {
        play->grid[it] = xi_v4_create(0.2f, 0.2f, 0.2f, 1.0f);
    }

    LD_PlacedItem *item = play->placed_items;
    while (item != 0) {
        LD_PlacedItem *remove = item;
        item = remove->next;

        LD_PlacedItemRemove(play, remove);
    }

    play->placed_items = 0;
}

function void LD_SolutionGenerate(LD_ModePlay *play) {
    u32 missing   = 100;
    u32 occupancy = 0;

    while (missing >= 4) {
        LD_SolutionClear(play);

        occupancy = play->occupancy;

        LD_ShapeType shape = LD_SHAPE_TYPE_T + xi_rng_choice_u32(&play->rng, LD_SHAPE_TYPE_COUNT - LD_SHAPE_TYPE_T);
        LD_ShapeType last_shape;

        b32 failed = false;
        while (!failed) {
            b32 placed = 0;

            u32 count = 0;

            u32 x;
            u32 y;

            while (placed == 0) {
                x = xi_rng_choice_u32(&play->rng, GRID_WIDTH);
                y = xi_rng_choice_u32(&play->rng, GRID_HEIGHT);

                placed = LD_PlaceShape(play, occupancy, shape, x, y);

                last_shape = shape;

                while (shape == last_shape) {
                    // don't place two shapes of the same type back to back
                    //
                    shape = xi_rng_choice_u32(&play->rng, LD_SHAPE_TYPE_COUNT);
                }


                count += 1;
                if (count >= 100) {
                    failed = true;
                    break;
                }
            }

            occupancy |= placed;
        }

        missing = __popcnt(~occupancy) - 2;
    }

    xi_log(&play->logger, "generation", "missing spaces: %d : 0x%x", __popcnt(~occupancy) - 2, occupancy);
}

function void LD_SolutionStep(LD_ModePlay *play) {
    b32 placed = false;
    u32 count = 0;
    while (placed == 0) {
        u32 x = xi_rng_choice_u32(&play->rng, GRID_WIDTH);
        u32 y = xi_rng_choice_u32(&play->rng, GRID_HEIGHT);

        u32 shape = xi_rng_choice_u32(&play->rng, LD_SHAPE_TYPE_COUNT);

        placed = LD_PlaceShape(play, play->step_occupancy, shape, x, y);

        if (placed != 0) {
            xi_log(&play->logger, "step", "placed shape %d", shape);
        }

        count += 1;
        if ((placed == 0) && count >= 500) {
            xi_log(&play->logger, "step", "unable to place shape");
            break;
        }
    }

    play->step_occupancy |= placed;
}

function u32 LD_ShapeToOccupancy(u32 x, u32 y, LD_ShapeType type) {
    u32 result  = LD_CoordToOccupancy(x, y);

    LD_ShapeInfo *info = &shapes[type];
    for (u32 it = 0; it < info->offset_count; ++it) {
        u32 ox = x + info->offsets[it].x;
        u32 oy = y + info->offsets[it].y;

        result |= LD_CoordToOccupancy(ox, oy);
    }

    return result;
}

function void LD_ModePlayInit(LD_Context *ld) {
    xi_arena_reset(&ld->mode_arena);

    xiContext *xi = ld->xi;

    LD_ModePlay *play = xi_arena_push_array(&ld->mode_arena, LD_ModePlay, 1);
    if (play) {
        play->ld    = ld;
        play->arena = &ld->arena;

        xi_rng_seed(&play->rng, xi->time.ticks);
        xi_logger_create(play->arena, &play->logger, xi->system.out, XI_KB(128));

        play->grid = xi_arena_push_array(play->arena, v4, 32);

        f32 aspect = (xi->window.width / (f32) xi->window.height);

        xi_camera_transform_get_from_axes(&play->camera, aspect,
                xi_v3_create(1, 0, 0), xi_v3_create(0, 1, 0), xi_v3_create(0, 0, 1), xi_v3_create(0, 0, 5), 0);

        play->min  = LD_SHAPE_TYPE_I;
        play->max  = LD_SHAPE_TYPE_IR;

        play->type = LD_SHAPE_TYPE_I;

        LD_SolutionClear(play);

        ld->mode = LD_GAME_MODE_PLAY;
        ld->play = play;
    }
}

function void LD_ModePlayUpdate(LD_ModePlay *play, f32 dt) {
    LD_Context *ld = play->ld;
    xiContext  *xi = ld->xi;

    xiInputKeyboard *kb = &xi->keyboard;

    if (kb->keys['r'].pressed) {
        play->type += 1;
        if (play->type > play->max) { play->type = play->min; }
    }

    if (kb->keys['g'].pressed) { LD_SolutionGenerate(play); }
    if (kb->keys['s'].pressed) { LD_SolutionStep(play);     }
    if (kb->keys['c'].pressed) { LD_SolutionClear(play);    }

    if (kb->keys['='].pressed) {
        play->cindex += 1;
        if (play->cindex >= XI_ARRAY_SIZE(debug_colours)) { play->cindex = 0; }
    }

    f32 aspect = (xi->window.width / (f32) xi->window.height);

    xi_camera_transform_get_from_axes(&play->camera, aspect, xi_v3_create(1, 0, 0), xi_v3_create(0, 1, 0),
            xi_v3_create(0, 0, 1), xi_v3_create(0, 0, 5), 0);

    v2 mouse = xi->mouse.position.clip;
    v3 world = xi_unproject_xy(&play->camera, mouse);

    s32 mx = (s32) ((world.x / 0.5f) + 0.5f);
    s32 my = (s32) ((world.y / 0.5f) + 0.5f);

    if (mx >= 0 && mx < GRID_WIDTH) {
        if (my >= 0 && my < GRID_HEIGHT) {
            if (xi->mouse.left.pressed) {
                LD_ShapeInfo *info = &shapes[play->type];
                u32 shapeoc = LD_ShapeToOccupancy(mx, my, play->type);

                b32 valid = (__popcnt(shapeoc) == (info->offset_count + 1)) && ((play->occupancy & shapeoc) == 0);
                if (valid) {
                    play->occupancy |= shapeoc;

                    // @incomplete: this _creates_ a new placed item, we really want to have a list of
                    // placable items generated at the beginning and the player drags them onto the grid,
                    // this will keep them in the 'placed' item category but will also add them to the
                    // bag
                    //

                    LD_PlacedItem *item = LD_PlacedItemGet(play);
                    XI_ASSERT(item != 0);

                    item->x     = (f32) mx;
                    item->y     = (f32) my;
                    item->shape = play->type;
                    item->info  = &items[0]; // hardcode fishing rod
                    item->next  = play->placed_items;

                    play->placed_items = item;
                }
            }
        }
    }

    xi_logger_flush(&play->logger);
}

typedef struct LD_VisualInfo LD_VisualInfo;
struct LD_VisualInfo {
    v2  offset;
    f32 angle;
};

function LD_VisualInfo LD_VisualInfoForShape(LD_ShapeType type) {
    LD_VisualInfo result = { 0 };

    switch (type) {
        case LD_SHAPE_TYPE_I:  {} break;
        case LD_SHAPE_TYPE_IR: { result.angle = PI / 2; } break;

        case LD_SHAPE_TYPE_L:  {
            result.offset.x = 0.5f;
            result.offset.y = 0.5f;
            result.angle    = PI / 2;
        }
        break;
        case LD_SHAPE_TYPE_J:  {
            result.offset.x = -0.5f;
            result.offset.y =  0.5f;
            result.angle    =  PI;
        }
        break;
        case LD_SHAPE_TYPE_LR: {
            result.offset.x = -0.5f;
            result.offset.y = -0.5f;
            result.angle    = -PI / 2;
        }
        break;
        case LD_SHAPE_TYPE_JR: {
            result.offset.x =  0.5f;
            result.offset.y = -0.5f;
            result.angle    =  0;
        }
        break;

        case LD_SHAPE_TYPE_O: {
            result.offset.x = 0.5f;
            result.angle    = PI / 2;
        }
        break;
        case LD_SHAPE_TYPE_OR: { result.offset.y = 0.5f; } break;

        case LD_SHAPE_TYPE_T: {
            result.offset.y = -0.5f;
            result.angle    = PI;
        }
        break;
        case LD_SHAPE_TYPE_TR: {
            result.offset.x =  0.5f;
            result.angle    = -PI / 2;
        }
        break;
        case LD_SHAPE_TYPE_TU: { result.offset.y = 0.5f; } break;
        case LD_SHAPE_TYPE_TL: {
            result.offset.x = -0.5f;
            result.angle    =  PI / 2;
        }
        break;

        case LD_SHAPE_TYPE_S: { result.offset.y = 0.5f; } break;
        case LD_SHAPE_TYPE_SR: {
            result.offset.x = -0.5f;
            result.angle    = -PI / 2;
        }
        break;
        case LD_SHAPE_TYPE_Z:  { result.offset.y = -0.5f; } break;
        case LD_SHAPE_TYPE_ZR: {
            result.offset.x =  0.5f;
            result.angle    = -PI / 2;
        }
        break;
    }

    return result;
}

function void LD_ModePlayRender(LD_ModePlay *play, xiRenderer *renderer) {
    LD_Context *ld = play->ld;
    xiContext  *xi = ld->xi;

    xi_camera_transform_set_from_axes(renderer, xi_v3_create(1, 0, 0), xi_v3_create(0, 1, 0),
            xi_v3_create(0, 0, 1), xi_v3_create(0, 0, 5), 0);

    v2 mouse = xi->mouse.position.clip;
    v3 world = xi_unproject_xy(&renderer->camera, mouse);

    f32 dim = 0.5f;

    s32 mx = (s32) ((world.x / dim) + 0.5f);
    s32 my = (s32) ((world.y / dim) + 0.5f);

    f32 xp = 0;
    f32 yp = 0;

    f32 grid_offset = 0.0f;

    u32 shapeoc = LD_ShapeToOccupancy(mx, my, play->type);

    b32 shape_drawn = false;

    for (u32 y = 0; y < GRID_HEIGHT; ++y) {
        for (u32 x = 0; x < GRID_WIDTH; ++x) {
            if ((y == (GRID_HEIGHT - 1)) && ((x == 0) || (x == (GRID_WIDTH - 1)))) {
                xp += dim;
                xp += grid_offset;

                continue;
            }

            u32 coord = LD_CoordToOccupancy(x, y);

            v4 colour = play->grid[(y * GRID_WIDTH) + x];
            if (coord & shapeoc) {
                LD_ShapeInfo *info = &shapes[play->type];

                b32 valid = (__popcnt(shapeoc) == (info->offset_count + 1)) && ((play->occupancy & shapeoc) == 0);
                if (!valid) {
                    colour = xi_v4_create(1, 0, 0, 1);
                }
                else {
                    colour = xi_v4_create(0.05f, 0.1f, 0.95f, 1.0f);
                }
            }

            xi_quad_draw_xy(renderer, colour, xi_v2_create(xp, yp), xi_v2_create(dim, dim), 0);

            if (!shape_drawn && (coord & shapeoc)) {
                xiImageHandle handle = xi_image_get_by_name(&xi->assets, "torch");
                xiaImageInfo *info   = xi_image_info_get(&xi->assets, handle);
                LD_VisualInfo visual = LD_VisualInfoForShape(play->type);

                f32 scale = 0.5f * (XI_MAX(info->width, info->height) / 32.0f);

                v2 p = xi_v2_create(dim * (mx + visual.offset.x), dim * (my + visual.offset.y));
                v4 c = xi_v4_create(1, 1, 1, 1);
                xi_coloured_sprite_draw_xy_scaled(renderer, handle, c, p, scale, visual.angle);
            }

            xp += dim;
            xp += grid_offset;
        }

        xp = 0;

        yp += dim;
        yp += grid_offset;
    }

    {
        // draw placed items
        //
        LD_PlacedItem *item = play->placed_items;
        while (item != 0) {
            xiImageHandle img    = xi_image_get_by_name(&xi->assets, item->info->name);
            LD_VisualInfo visual = LD_VisualInfoForShape(item->shape);

            f32 dim   = 0.5f;
            f32 scale = 1.5f;

            v2 p = xi_v2_create(dim * (item->x + visual.offset.x), dim * (item->y + visual.offset.y));
            xi_sprite_draw_xy_scaled(renderer, img, p, scale, visual.angle);

            item = item->next;
        }
    }

    rect3 bounds = xi_camera_bounds_get(&renderer->camera);

    const char *rooms[] = { "bedroom", "bathroom", "kitchen", "kitchen", "kitchen", "bedroom" };

    for (u32 it = 0; it < XI_ARRAY_SIZE(rooms); ++it) {
        xiImageHandle img = xi_image_get_by_name(&xi->assets, rooms[it]);

        f32 scale = 1.89f; // @hack: why is this value good?

        v2 p = xi_v2_create(bounds.min.x + (0.5f * scale) + (it * scale), bounds.min.y + (0.25f * scale));
        xi_sprite_draw_xy_scaled(renderer, img, p, scale, 0);
    }

    if (0) {
        LD_VisualInfo visual = LD_VisualInfoForShape(play->type);

        xiImageHandle img = xi_image_get_by_name(&xi->assets, "rod");
        xi_sprite_draw_xy_scaled(renderer, img, world.xy, 1.5f, visual.angle);
    }

    xi_quad_draw_xy(renderer, xi_v4_create(1, 0, 1, 1), world.xy, xi_v2_create(0.1f, 0.1f), 0);


}
