#include "ld_mode_play.h"

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

function void LD_ShapeRotationRangeSet(LD_PlacedItem *item) {
    switch (item->shape) {
        case LD_SHAPE_TYPE_I:
        case LD_SHAPE_TYPE_IR: {
            item->rotation_min = LD_SHAPE_TYPE_I;
            item->rotation_max = LD_SHAPE_TYPE_IR;
        }
        break;

        case LD_SHAPE_TYPE_L:
        case LD_SHAPE_TYPE_J:
        case LD_SHAPE_TYPE_LR:
        case LD_SHAPE_TYPE_JR: {
            item->rotation_min = LD_SHAPE_TYPE_L;
            item->rotation_max = LD_SHAPE_TYPE_JR;
        }
        break;

        case LD_SHAPE_TYPE_O:
        case LD_SHAPE_TYPE_OR: {
            item->rotation_min = LD_SHAPE_TYPE_O;
            item->rotation_max = LD_SHAPE_TYPE_OR;
        }
        break;

        case LD_SHAPE_TYPE_T:
        case LD_SHAPE_TYPE_TR:
        case LD_SHAPE_TYPE_TU:
        case LD_SHAPE_TYPE_TL: {
            item->rotation_min = LD_SHAPE_TYPE_T;
            item->rotation_max = LD_SHAPE_TYPE_TL;
        }
        break;

        case LD_SHAPE_TYPE_S:
        case LD_SHAPE_TYPE_SR: {
            item->rotation_min = LD_SHAPE_TYPE_S;
            item->rotation_max = LD_SHAPE_TYPE_SR;
        }
        break;

        case LD_SHAPE_TYPE_Z:
        case LD_SHAPE_TYPE_ZR: {
            item->rotation_min = LD_SHAPE_TYPE_Z;
            item->rotation_max = LD_SHAPE_TYPE_ZR;
        }
        break;
    }
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

        item->x     = xi_rng_range_f32(&play->rng, bounds.min.x + 1, 0.5f * (bounds.max.x + bounds.min.x) - 1.125f);
        item->y     = xi_rng_range_f32(&play->rng, bounds.min.y + 1.70f, 0.37f * (bounds.max.y - bounds.min.y));
        item->shape = base;
        item->next  = play->placed_items;

        LD_ShapeRotationRangeSet(item);

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

    for (u32 it = 0; it < 32; ++it) { play->grid[it] = xi_v4_create(1, 1, 1, 1); }

    LD_PlacedItem *item = play->placed_items;
    while (item != 0) {
        LD_PlacedItem *remove = item;
        item = remove->next;

        LD_PlacedItemRemove(play, remove);
    }

    play->placed_items = 0;
    play->bag_items = 0;
}

function void LD_SolutionGenerate(LD_ModePlay *play) {
    u32 missing   = 100;
    u32 occupancy = 0;

    LD_ShapeType distribution[] = {
        LD_SHAPE_TYPE_I,
        LD_SHAPE_TYPE_IR,
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
        LD_SHAPE_TYPE_S,
        LD_SHAPE_TYPE_SR,

        LD_SHAPE_TYPE_Z,
        LD_SHAPE_TYPE_ZR,
        LD_SHAPE_TYPE_Z,
        LD_SHAPE_TYPE_ZR,
    };

    u32 route_index = xi_rng_choice_u32(&play->rng, XI_ARRAY_SIZE(map_routes));
    LD_MapRoute *route = &map_routes[route_index];

    u32 placed_count;
    while (missing > 4 || (placed_count < route->node_count)) {
        LD_SolutionClear(play);

        occupancy = play->occupancy;

        u32 index = xi_rng_choice_u32(&play->rng, XI_ARRAY_SIZE(distribution));
        LD_ShapeType shape = distribution[index];
        // LD_SHAPE_TYPE_T + xi_rng_choice_u32(&play->rng, LD_SHAPE_TYPE_COUNT - LD_SHAPE_TYPE_T);

        LD_ShapeType last_shape;

        placed_count = 0;

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
                if (placed != 0) { placed_count += 1; }

                last_shape = shape;
                while (shape == last_shape) {
                    // don't place two shapes of the same type back to back
                    //
                    index = xi_rng_choice_u32(&play->rng, XI_ARRAY_SIZE(distribution));
                    shape = distribution[index]; // xi_rng_choice_u32(&play->rng, LD_SHAPE_TYPE_COUNT);
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

    u32 item_count = 0;
    LD_PlacedItem *item = play->placed_items;
    while (item != 0) {
        item_count += 1;
        item = item->next;
    }

    play->selected = route;

    xi_log(&play->logger, "gen", "%d items were placed", item_count);

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

function void LD_DrawItems(xiRenderer *renderer, LD_ShapePositions *invShapes){
    v2 bagCenter = xi_v2_create(-4.5f,1.5f);
    LD_ShapeType bagShapes[7] = {0,2,4,6,8,12,13};
    f32 quadDim = 0.3f;
    f32 currentRowLength = 0;
    f32 rows = 0;
    for(u32 i = 0; i < 7; i++){
        LD_ShapeInfo quads = shapes[bagShapes[i]];
        invShapes[i].quad_number = quads.offset_count + 1;
        if(currentRowLength > 3){
            currentRowLength = 0;
            rows -= quadDim*3;
        }
        v2 shapeCenter = xi_v2_add(bagCenter, xi_v2_create(currentRowLength,rows));
        xi_quad_draw_xy(renderer, xi_v4_create(1, 0, 1, 1), shapeCenter, xi_v2_create(quadDim, quadDim), 0);
        invShapes[i].positions[0] = shapeCenter;
        currentRowLength+=quadDim;
        for(u32 j = 0; j < quads.offset_count; j++){
            f32 xp = shapeCenter.x;
            f32 yp = shapeCenter.y;
            xp += quadDim * quads.offsets[j].x;
            yp += quadDim * quads.offsets[j].y;
            v2 pos = xi_v2_create(xp,yp);
            invShapes[i].positions[j+1] = pos;
            xi_quad_draw_xy(renderer, xi_v4_create(1, 0, 1, 1), pos, xi_v2_create(quadDim, quadDim), 0);
            currentRowLength += quadDim * quads.offsets[j].x;
        }
        currentRowLength += quadDim + 0.15f;
    }
}

function void LD_Pickup(v2 cursorPos, LD_ModePlay *play){
    if(play->itemSelected){
        return;
    }
    for(u32 i = 0; i < 7; i++){
        LD_ShapePositions shape = play->invShapes[i];
        for(u32 j = 0; j < shape.quad_number; j++){
            xi_rect2 bounds;
            bounds.min = xi_v2_create(shape.positions[j].x-0.15f,shape.positions[j].y-0.15f );
            bounds.max = xi_v2_create(shape.positions[j].x+0.15f,shape.positions[j].y+0.15f );
            if(cursorPos.x > bounds.min.x && cursorPos.x <= bounds.max.x && cursorPos.y > bounds.min.y && cursorPos.y <= bounds.max.y){
                play->selected_shape = shape;
                play->itemSelected = true;
                return;
            }
        }
    }
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

        play->hero = xi_animation_get_by_name(&xi->assets, "hero");
        play->heroFlame = xi_animation_get_by_name(&xi->assets, "hero_flame");

        play->grid = xi_arena_push_array(play->arena, v4, 32);

        f32 aspect = (xi->window.width / (f32) xi->window.height);

        play->puzzle_timer = 60.0f;

        play->map_hovered = true;

        xi_camera_transform_get_from_axes(&play->camera, aspect,
                xi_v3_create(1, 0, 0), xi_v3_create(0, 1, 0), xi_v3_create(0, 0, 1), xi_v3_create(0, 0, 5), 0);

        LD_SolutionClear(play);

        ld->mode = LD_GAME_MODE_PLAY;
        ld->play = play;
    }
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

function rect2 LD_HitboxForPlacedItem(xiContext *xi, LD_PlacedItem *item) {
    rect2 result = { 0 };

    xiImageHandle img    = xi_image_get_by_name(&xi->assets, item->info->name);
    xiaImageInfo *info   = xi_image_info_get(&xi->assets, img);
    LD_VisualInfo visual = LD_VisualInfoForShape(item->shape);

    f32 scale = GRID_TILE_DIM * (XI_MAX(info->width, info->height) / 32.0f);
    v2 dim;
    if (info->width > info->height) {
        dim.x = 0.5f * scale;
        dim.y = 0.5f * scale * (info->height / (f32) info->width);
    }
    else {
        dim.x = 0.5f * scale * (info->width / (f32) info->height);
        dim.y = 0.5f * scale;
    }

    m2x2 rot = xi_m2x2_from_radians(visual.angle);

    v2 p   = xi_v2_create(item->x, item->y);
    v2 min = xi_v2_add(p, xi_m2x2_mul_v2(rot, xi_v2_neg(dim)));
    v2 max = xi_v2_add(p, xi_m2x2_mul_v2(rot, dim));

    result.min.x = XI_MIN(min.x, max.x);
    result.min.y = XI_MIN(min.y, max.y);

    result.max.x = XI_MAX(min.x, max.x);
    result.max.y = XI_MAX(min.y, max.y);

    return result;
}

function b32 LD_PointInRect(rect2 r, v2 p) {
    b32 result =
        (r.min.x <= p.x) && (p.x <= r.max.x) &&
        (r.min.y <= p.y) && (p.y <= r.max.y);

    return result;
}

function v2s LD_MouseToGrid(v2 mouse) {
    v2s result = { 0 };

    v2 mouse_grid;
    mouse_grid.x = ((mouse.x - GRID_BASE_X) / GRID_TILE_DIM);
    mouse_grid.y = ((mouse.y - GRID_BASE_Y) / GRID_TILE_DIM);

    if (mouse_grid.x >= 0) { result.x = (s32) (mouse_grid.x + 0.5f); }
    else { result.x = (s32) (mouse_grid.x - 0.5f); }

    if (mouse_grid.y >= 0) { result.y = (s32) (mouse_grid.y + 0.5f); }
    else { result.y = (s32) (mouse_grid.y - 0.5f); }

    return result;
}

function void LD_ModePlayUpdate(LD_ModePlay *play, f32 dt) {
    LD_Context *ld = play->ld;
    xiContext  *xi = ld->xi;

    xiInputKeyboard *kb = &xi->keyboard;

    if (kb->keys['g'].pressed) { LD_SolutionGenerate(play); }
    if (kb->keys['s'].pressed) { LD_SolutionStep(play);     }
    if (kb->keys['c'].pressed) { LD_SolutionClear(play);    }
    if (kb->keys['i'].pressed) { play->bagOpen = true;      }
    if (kb->keys['m'].pressed) { play->map_open = !play->map_open; }

    play->puzzle_timer -= dt;

    if (play->map_open && kb->keys['q'].pressed) {
        if (play->recording) {
            xiArena *temp = xi_temp_get();

            xi_buffer buffer;
            buffer.used  = 0;
            buffer.limit = 1024;
            buffer.data  = xi_arena_push_array(temp, u8, buffer.limit);

            LD_MapRoute *route = &play->route;

            xi_str_format_buffer(&buffer, "{ LD_MAP_DESTINATION_, %d, { ", route->node_count);

            for (u32 it = 0; it < route->node_count; ++it) {
                xi_str_format_buffer(&buffer, "{ %ff, %ff }, ", route->nodes[it].x, route->nodes[it].y);
            }

            xi_str_format_buffer(&buffer, "},");

            xi_log(&play->logger, "route", "%.*s", xi_str_unpack(buffer.str));
        }

        play->route.node_count = 0;
        play->recording = !play->recording;
    }

    play->map_timer += dt;

    if (kb->keys['='].pressed) {
        play->cindex += 1;
        if (play->cindex >= XI_ARRAY_SIZE(debug_colours)) { play->cindex = 0; }
    }

    f32 aspect = (xi->window.width / (f32) xi->window.height);

    xi_camera_transform_get_from_axes(&play->camera, aspect, xi_v3_create(1, 0, 0), xi_v3_create(0, 1, 0),
            xi_v3_create(0, 0, 1), xi_v3_create(0, 0, 5), 0);

    xiInputMouse *mouse = &xi->mouse;

    v3 world = xi_unproject_xy(&play->camera, mouse->position.clip);

    if (play->map_open && play->recording && mouse->left.pressed) {
        LD_MapRoute *route = &play->route;

        if (route->node_count < XI_ARRAY_SIZE(route->nodes)) {
            route->nodes[route->node_count++] = world.xy;
        }
    }

    v2s mouse_grid = LD_MouseToGrid(world.xy);

    if (mouse->left.pressed) {
        // just clicked
        //
        // loop through all of the placed items and check if the mouse is hovering over them,
        // if they are pick them up
        //
        LD_PlacedItem *item = play->placed_items;
        while (item != 0) {
            // @incomplete: if the item is in the bag it must be removed!
            //
            if (!item->placed) {
                rect2 hitbox = LD_HitboxForPlacedItem(xi, item);
                if (LD_PointInRect(hitbox, world.xy)) {
                    item->pick_x = item->x;
                    item->pick_y = item->y;
                    play->picked = item;

                    break;
                }
            }
            else {
                // is placed see if the mouse is within the grid square defined by the shape of this item
                // if it is remove from the bag and
                //
                if (item->x == mouse_grid.x && item->y == mouse_grid.y) {
                    play->picked     = item;
                    play->occupancy &= ~item->occupancy;

                    item->placed    = false;
                    item->occupancy = 0;
                    break;
                }
                else {
                    LD_ShapeInfo *shape = &shapes[item->shape];
                    for (u32 it = 0; it < shape->offset_count; ++it) {
                        v2s o = shape->offsets[it];
                        if (((item->x + o.x) == mouse_grid.x) && ((item->y + o.y) == mouse_grid.y)) {
                            play->picked     = item;
                            play->occupancy &= ~item->occupancy;

                            item->placed    = false;
                            item->occupancy = 0;

                            break;
                        }
                    }
                }
            }

            item = item->next;
        }

        if (play->picked) {
            play->picked->x = world.x;
            play->picked->y = world.y;
        }
    }
    else if (play->picked) {
        LD_PlacedItem *picked = play->picked;

        if (mouse->left.released) {
            b32 valid_placement = false;
            u32 occupancy = 0;

            if (mouse_grid.x >= 0 && mouse_grid.x < GRID_WIDTH) {
                if (mouse_grid.y >= 0 && mouse_grid.y < GRID_HEIGHT) {
                    LD_ShapeInfo *info = &shapes[picked->shape];
                    occupancy = LD_ShapeToOccupancy(mouse_grid.x, mouse_grid.y, picked->shape);
                    valid_placement = (__popcnt(occupancy) == (info->offset_count + 1)) && ((play->occupancy & occupancy) == 0);
                }
            }

            picked->placed = valid_placement;

            if (!valid_placement) {
                if (picked->x > 0) {
                    picked->x = picked->pick_x;
                    picked->y = picked->pick_y;
                }
                else {
                    LD_VisualInfo visual = LD_VisualInfoForShape(picked->shape);

                    picked->x = picked->x + (GRID_TILE_DIM * visual.offset.x);
                    picked->y = picked->y + (GRID_TILE_DIM * visual.offset.y);

                }
            }
            else {
                // valid so add it to the bag items
                //
                picked->x = (f32) mouse_grid.x;
                picked->y = (f32) mouse_grid.y;

                play->occupancy  |= occupancy;
                picked->occupancy = occupancy;

                picked->next_bag = play->bag_items;
                play->bag_items  = picked;
            }

            play->picked = 0;
        }
        else {
            picked->x = world.x;
            picked->y = world.y;

            if (kb->keys['r'].pressed || mouse->right.pressed) {
                picked->shape += 1;
                if (picked->shape > picked->rotation_max) { picked->shape = picked->rotation_min; }
            }
        }
    }

    // if (xi->mouse.left.down && play->bagOpen) { LD_Pickup(world.xy, play); }
    // if (xi->mouse.left.released){ play->itemSelected = false; }


    xi_animation_update(&play->hero, dt);
    xi_animation_update(&play->heroFlame, dt);


#if 0
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
#endif

    xi_logger_flush(&play->logger);
}

function void LD_ModePlayRender(LD_ModePlay *play, xiRenderer *renderer) {
    LD_Context *ld = play->ld;
    xiContext  *xi = ld->xi;

    xi_camera_transform_set_from_axes(renderer, xi_v3_create(1, 0, 0), xi_v3_create(0, 1, 0),
            xi_v3_create(0, 0, 1), xi_v3_create(0, 0, 5), 0);

    v2 mouse = xi->mouse.position.clip;
    v3 world = xi_unproject_xy(&renderer->camera, mouse);

    if(play->bagOpen){
        LD_DrawItems(renderer, play->invShapes);
    }

    if(play->itemSelected){
        u32 quads = play->selected_shape.quad_number;
        for(u32 i = 0; i < quads; i++){
            v2 netPos = xi_v2_create(play->selected_shape.positions[i].x - play->selected_shape.positions[0].x, play->selected_shape.positions[i].y - play->selected_shape.positions[0].y);
            v2 pos = xi_v2_create(world.x + netPos.x, world.y + netPos.y);
            xi_quad_draw_xy(renderer, xi_v4_create(1,0,0,1), pos, xi_v2_create(0.3f,0.3f), 0);
        }
    }

    //
    // draw bag with background
    //

    rect3 bounds = xi_camera_bounds_get(&renderer->camera);
    {
        xiImageHandle image = xi_image_get_by_name(&xi->assets, "bag_background");

        v2 screen_centre = xi_v2_mul_f32(xi_v2_add(bounds.max.xy, bounds.min.xy), 0.5f);
        v2 screen_dim    = xi_v2_sub(bounds.max.xy, bounds.min.xy);

        xi_quad_draw_xy(renderer, xi_v4_create(1, 0, 0, 1), screen_centre, xi_v2_create(0.25f, screen_dim.y), 0);

        v2 p;
        p.x = screen_centre.x + (0.25f * screen_dim.x) + 0.05f;
        p.y = screen_centre.y + 0.48f;

        xi_sprite_draw_xy_scaled(renderer, image, p, 5.8f, 0);
    }

    {
        v2 min;
        min.x = bounds.min.x + 1.0f;
        min.y = bounds.min.y + 1.70f;

        v2 max;
        max.x = 0.5f * (bounds.min.x + bounds.max.x) - 1.125f;
        max.y = 0.37f * (bounds.max.y - bounds.min.y);

        v2 cc = xi_v2_mul_f32(xi_v2_add(min, max), 0.5f);
        v2 dd = xi_v2_sub(max, min);

        xi_quad_draw_xy(renderer, xi_v4_create(0.25, 0.25, 0.25, 1.0f), cc, dd, 0);
    }

    //
    // draw grid of bag slots
    //

    xiImageHandle slot = xi_image_get_by_name(&xi->assets, "slot");

    f32 xp = GRID_BASE_X;
    f32 yp = GRID_BASE_Y;

    for (u32 y = 0; y < GRID_HEIGHT; ++y) {
        for (u32 x = 0; x < GRID_WIDTH; ++x) {
            if ((y == (GRID_HEIGHT - 1)) && ((x == 0) || (x == (GRID_WIDTH - 1)))) {
                // skip the top corners
                //
                xp += GRID_TILE_DIM;
                continue;
            }

            v4 colour = play->grid[(y * GRID_WIDTH) + x];
            colour.a *= 0.5f;

            xi_coloured_sprite_draw_xy_scaled(renderer, slot, colour, xi_v2_create(xp, yp), GRID_TILE_DIM, 0);

            xp += GRID_TILE_DIM;
        }

        xp  = GRID_BASE_X;
        yp += GRID_TILE_DIM;
    }

    //
    // draw the item placement snapped to the grid if it is within bounds
    //

    v2s mouse_grid = LD_MouseToGrid(world.xy);

    if (play->picked) {
        LD_PlacedItem *picked = play->picked;
        u32 o = LD_ShapeToOccupancy(mouse_grid.x, mouse_grid.y, picked->shape);
        if (o != 0) {
            LD_ShapeInfo *shape  = &shapes[picked->shape];

            xiImageHandle handle = xi_image_get_by_name(&xi->assets, picked->info->name);
            xiaImageInfo *info   = xi_image_info_get(&xi->assets, handle);
            LD_VisualInfo visual = LD_VisualInfoForShape(picked->shape);

            b32 invalid = (__popcnt(o) != (shape->offset_count + 1)) || (play->occupancy & o);

            f32 scale = 0.5f * (XI_MAX(info->width, info->height) / 32.0f);

            v2 p;
            p.x = (GRID_TILE_DIM * (mouse_grid.x + visual.offset.x)) + GRID_BASE_X;
            p.y = (GRID_TILE_DIM * (mouse_grid.y + visual.offset.y)) + GRID_BASE_Y;

            v4 c = invalid ? xi_v4_create(1, 0, 0, 0.4f) : xi_v4_create(1, 1, 1, 0.4f);
            xi_coloured_sprite_draw_xy_scaled(renderer, handle, c, p, scale, visual.angle);
        }
    }

    // @debug: where the grid slot is
    //
    // xi_quad_draw_xy(renderer, xi_v4_create(1, 1, 0, 1),
    //       xi_v2_create((GRID_TILE_DIM * gm) + GRID_BASE_X, (GRID_TILE_DIM * my) + GRID_BASE_Y), xi_v2_create(0.1f, 0.1f), 0);
    {
        // draw placed items
        //
        LD_PlacedItem *item = play->placed_items;
        while (item != 0) {
            xiImageHandle img    = xi_image_get_by_name(&xi->assets, item->info->name);
            xiaImageInfo *info   = xi_image_info_get(&xi->assets, img);
            LD_VisualInfo visual = LD_VisualInfoForShape(item->shape);

            f32 scale = GRID_TILE_DIM * (XI_MAX(info->width, info->height) / 32.0f);

            v2 p;
            if (item->placed) {
                p.x = (GRID_TILE_DIM * (item->x + visual.offset.x)) + GRID_BASE_X;
                p.y = (GRID_TILE_DIM * (item->y + visual.offset.y)) + GRID_BASE_Y;
            }
            else {
               p = xi_v2_create(item->x, item->y);
            }

            if (item == play->picked) {
                p.x += GRID_TILE_DIM * visual.offset.x;
                p.y += GRID_TILE_DIM * visual.offset.y;
            }

            if (!item->placed) {
                rect2 box = LD_HitboxForPlacedItem(xi, item);

                v4 bc = xi_v4_create(0, 1, 1, 1);
                if (world.x >= box.min.x && world.x <= box.max.x) {
                    if (world.y >= box.min.y && world.y <= box.max.y) {
                        bc = xi_v4_create(0, 1, 0, 1);
                    }
                }

                xi_quad_outline_draw_xy(renderer, bc, p, xi_v2_sub(box.max, box.min), 0, 0.01f);
            }

            xi_sprite_draw_xy_scaled(renderer, img, p, scale, visual.angle);

            item = item->next;
        }
    }

    const char *rooms[] = { "bedroom", "bathroom", "kitchen", "kitchen", "kitchen", "bedroom" };

    for (u32 it = 0; it < XI_ARRAY_SIZE(rooms); ++it) {
        xiImageHandle img = xi_image_get_by_name(&xi->assets, rooms[it]);

        f32 scale = 1.89f; // @hack: why is this value good?

        v2 p = xi_v2_create(bounds.min.x + (0.5f * scale) + (it * scale), bounds.min.y + (0.25f * scale));
        xi_sprite_draw_xy_scaled(renderer, img, p, scale, 0);
    }

    f32 guy_pos = 10.5f;
    if(play->puzzle_timer > 0){
        guy_pos = (60 - play->puzzle_timer)*guy_pos/60;
    }
    xiImageHandle hero = xi_animation_current_frame_get(&play->hero);
    xi_sprite_draw_xy_scaled(renderer, hero, xi_v2_add(bounds.min.xy, xi_v2_create((guy_pos + 0.8f), 0.4f)), 0.4f, 0);

    xiImageHandle heroFlame = xi_animation_current_frame_get(&play->heroFlame);
    xi_sprite_draw_xy_scaled(renderer, heroFlame, xi_v2_add(bounds.min.xy, xi_v2_create(guy_pos + 0.55f, 0.3f)), 0.2f, 0);

    {
        xiImageHandle map = xi_image_get_by_name(&xi->assets, "map");

        f32 scale = 1.0;
        f32 angle = 0;
        if (play->map_hovered) {
            angle  = 0.1f * xi_sin(play->map_timer);
            scale += 0.05f * xi_sin(play->map_timer * 3.8f);
        }

        xi_sprite_draw_xy_scaled(renderer, map, xi_v2_sub(bounds.max.xy, xi_v2_create(1.0, 1.0)), scale, angle);
    }

    //
    // drawing for the map screen
    //
    // @todo: should probably do this first and not draw the rest of the crap if this is covering the
    // screen
    //

    if (play->map_open) {
        xiImageHandle img = xi_image_get_by_name(&xi->assets, "map_screen");

        v2 map_dim    = xi_v2_sub(bounds.max.xy, bounds.min.xy);
        f32 map_scale = XI_MAX(map_dim.x, map_dim.y); // * (160.0f / 9.0f);

        xi_sprite_draw_xy_scaled(renderer, img, xi_v2_create(0, 0), map_scale, 0);

        if (play->recording) {
            LD_MapRoute *route = &play->route;
            for (u32 it = 0; it < route->node_count; ++it) {
                xi_quad_draw_xy(renderer, xi_v4_create(0, 1, 0, 1), route->nodes[it],
                        xi_v2_create(0.1f, 0.1f), 0);
            }
        }
        else if (play->selected != 0) {
            LD_MapRoute *route = play->selected;
            for (u32 it = 0; it < route->node_count; ++it) {
                xi_quad_draw_xy(renderer, xi_v4_create(0, 1, 0, 1), route->nodes[it],
                        xi_v2_create(0.1f, 0.1f), 0);
            }
        }
    }

    // mouse cursor
    //
    xi_quad_draw_xy(renderer, xi_v4_create(1, 0, 1, 1), world.xy, xi_v2_create(0.1f, 0.1f), 0);
}
