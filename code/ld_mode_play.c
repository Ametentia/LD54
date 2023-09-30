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

struct LD_ModePlay {
    LD_Context *ld;

    xiArena *arena;
    xiLogger logger;

    xiAnimation bottle;

    f32 timer;

    f32 angle;

    // [00][01][02][03][04][05]
    // [06][07][08][09][10][11]
    // [12][13][14][15][16][17]
    // [18][19][20][21][22][23]
    // [24][25][26][27][28][29]
    //
    u32 occupied;
    u32 visual; // the occupancy of the dragged shapes

    xiRandomState rng;

    u32 step_occupancy;
    u32 cindex;
    v4 *grid;
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
    LD_SHAPE_TYPE_LR,

    LD_SHAPE_TYPE_J,
    LD_SHAPE_TYPE_JR,

    LD_SHAPE_TYPE_O,
    LD_SHAPE_TYPE_OR,

    LD_SHAPE_TYPE_T,
    LD_SHAPE_TYPE_TR,
    LD_SHAPE_TYPE_TL,
    LD_SHAPE_TYPE_TU,

    LD_SHAPE_TYPE_S,
    LD_SHAPE_TYPE_Z,

    LD_SHAPE_TYPE_COUNT
};

typedef struct LD_ShapeInfo LD_ShapeInfo;
struct LD_ShapeInfo {
    u32 offset_count;
    v2s offsets[4];
};

global LD_ShapeInfo shapes[LD_SHAPE_TYPE_COUNT] = {
    2, { {  0, 1 }, {  0,  2 }            }, // I
    2, { {  1, 0 }, {  2,  0 }            }, // IR

    2, { {  1, 0 }, {  0,  1 }            }, // L
    2, { {  1, 0 }, {  1, -1 }            }, // LR
    2, { {  1, 0 }, {  1,  1 }            }, // J
    2, { { -1, 0 }, { -1, -1 }            }, // JR

    1, { {  1, 0 }                        }, // O
    1, { {  0, 1 }                        }, // OR

    3, { {  1, 0 }, { -1,  0 }, { 0, -1 } }, // T
    3, { {  0, 1 }, {  1,  0 }, { 0, -1 } }, // TR
    3, { {  0, 1 }, { -1,  0 }, { 0, -1 } }, // TL
    3, { {  1, 0 }, { -1,  0 }, { 0,  1 } }, // TU

    3, { {  1, 0 }, {  1,  1 }, { 2,  1 } }, // S
    3, { {  1, 0 }, {  1, -1 }, { 2, -1 } }, // Z
};

function u32 LD_PlaceShape(LD_ModePlay *play, u32 occupancy, LD_ShapeType type, u32 x, u32 y) {
    LD_ShapeInfo *info = &shapes[type];

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

        play->cindex += 1;
        play->cindex %= XI_ARRAY_SIZE(debug_colours);
    }
    else {
        result = 0;
    }

    return result;
}

function void LD_SolutionGenerate(LD_ModePlay *play) {
    play->step_occupancy = 0;

    u32 missing = 100;
    u32 occupancy;
    while (missing > 0) {
        occupancy = LD_CoordToOccupancy(0, GRID_HEIGHT - 1) | LD_CoordToOccupancy(GRID_WIDTH - 1, GRID_HEIGHT - 1);
        play->cindex = 0;

        for (u32 it = 0; it < 32; ++it) {
            play->grid[it] = xi_v4_create(0.2f, 0.2f, 0.2f, 1.0f);
        }

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

function void LD_SolutionClear(LD_ModePlay *play) {
    play->cindex = 0;
    play->step_occupancy = LD_CoordToOccupancy(0, GRID_HEIGHT - 1) | LD_CoordToOccupancy(GRID_WIDTH - 1, GRID_HEIGHT - 1);

    for (u32 it = 0; it < 32; ++it) {
        play->grid[it] = xi_v4_create(0.2f, 0.2f, 0.2f, 1.0f);
    }
}


function void LD_ModePlayInit(LD_Context *ld) {
    xi_arena_reset(&ld->mode_arena);

    xiContext *xi = ld->xi;

    LD_ModePlay *play = xi_arena_push_array(&ld->mode_arena, LD_ModePlay, 1);
    if (play) {
        play->ld = ld;
        play->arena = &ld->arena;

        play->bottle = xi_animation_get_by_name(&xi->assets, "bottle");

        play->grid = xi_arena_push_array(play->arena, v4, 32);

        xi_rng_seed(&play->rng, xi->time.ticks);

        xi_logger_create(play->arena, &play->logger, xi->system.out, XI_KB(128));

        // pre-initialise the occupancy to ignore the top two corners as they are not valid
        //
        play->occupied = 0;

        play->occupied |= LD_CoordToOccupancy(0, GRID_HEIGHT - 1);
        play->occupied |= LD_CoordToOccupancy(GRID_WIDTH - 1, GRID_HEIGHT - 1);

#if 0
        play->visual = 0;

        play->visual |= LD_CoordToOccupancy(0, GRID_HEIGHT - 1);
        play->visual |= LD_CoordToOccupancy(1, GRID_HEIGHT - 1);
        play->visual |= LD_CoordToOccupancy(0, GRID_HEIGHT - 2);
#endif

        ld->mode = LD_GAME_MODE_PLAY;
        ld->play = play;
    }
}

function void LD_ModePlayUpdate(LD_ModePlay *play, f32 dt) {
    LD_Context *ld = play->ld;
    xiContext  *xi = ld->xi;

    play->timer += dt;
#if 0

    if (play->timer > 0.25f) {
        xi_log(&play->logger, "test", "0x%x", play->occupied);

        play->occupied <<= 1;
        if (play->occupied == 0) { play->occupied = 1; }

        play->timer = 0;
    }
#endif
    xiInputKeyboard *kb = &xi->keyboard;

    if (kb->keys['r'].pressed) {
        play->angle += (PI / 2);
        if (play->angle >= (2 * PI)) { play->angle = 0; }
    }

    if (kb->keys['g'].pressed) { LD_SolutionGenerate(play); }
    if (kb->keys['s'].pressed) { LD_SolutionStep(play); }
    if (kb->keys['c'].pressed) { LD_SolutionClear(play); }



    if (kb->keys['='].pressed) {
        play->cindex += 1;
        if (play->cindex >= XI_ARRAY_SIZE(debug_colours)) { play->cindex = 0; }
    }

    f32 aspect = (xi->window.width / (f32) xi->window.height);
    xiCameraTransform cam;

    xi_camera_transform_get_from_axes(&cam, aspect, xi_v3_create(1, 0, 0), xi_v3_create(0, 1, 0),
            xi_v3_create(0, 0, 1), xi_v3_create(0, 0, 5), 0);

    v2 mouse = xi->mouse.position.clip;
    v3 world = xi_unproject_xy(&cam, mouse);


    s32 mx = (s32) ((world.x / 0.5f) + 0.5f);
    s32 my = (s32) ((world.y / 0.5f) + 0.5f);

    if (mx >= 0 && mx < GRID_WIDTH) {
        if (my >= 0 && my < GRID_HEIGHT) {
            if (xi->mouse.left.pressed) {
                play->grid[(my * GRID_WIDTH) + mx] = debug_colours[play->cindex];
            }
        }
    }



    xi_animation_update(&play->bottle, dt);

    xi_logger_flush(&play->logger);
}

function void LD_ModePlayRender(LD_ModePlay *play, xiRenderer *renderer) {
    LD_Context *ld = play->ld;
    xiContext  *xi = ld->xi;

    xi_camera_transform_set_from_axes(renderer, xi_v3_create(1, 0, 0), xi_v3_create(0, 1, 0),
            xi_v3_create(0, 0, 1), xi_v3_create(0, 0, 5), 0);

    v2 mouse = xi->mouse.position.clip;
    v3 world = xi_unproject_xy(&renderer->camera, mouse);

#if 0
    xiImageHandle image = xi_animation_current_frame_get(&play->bottle);
    xi_sprite_draw_xy_scaled(renderer, image, xi_v2_create(0, 0), 1.0f, 0);

    const char *names[] = { "testl", "testb", "tests", "testt" };

    f32 ix = -1.8f;

    for (u32 it = 0; it < 4; ++it) {
        xiImageHandle img = xi_image_get_by_name(&xi->assets, names[it]);

        xi_sprite_draw_xy_scaled(renderer, img, xi_v2_create(ix, 0), 1.0f, 0);

        ix += 1.2f;
    }
#endif

    s32 mx = (s32) ((world.x / 0.5f) + 0.5f);
    s32 my = (s32) ((world.y / 0.5f) + 0.5f);



    u32 mouse_o = 0;

    mouse_o |= LD_CoordToOccupancy(mx, my);
#if 0
    mouse_o |= LD_CoordToOccupancy(mx + (s32) xi_cos(play->angle), my + (s32) xi_sin(play->angle));
    mouse_o |= LD_CoordToOccupancy(mx - (s32) xi_sin(play->angle), my + (s32) xi_cos(play->angle));
#else
    mouse_o |= LD_CoordToOccupancy(mx - (s32) xi_sin(play->angle), my + (s32) xi_cos(play->angle));
    mouse_o |= LD_CoordToOccupancy(mx + (s32) xi_sin(play->angle), my - (s32) xi_cos(play->angle));
#endif

    f32 dim = 0.5f;

    f32 xp = 0;
    f32 yp = 0;

    f32 grid_offset = 0.015f;

    for (u32 y = 0; y < GRID_HEIGHT; ++y) {
        for (u32 x = 0; x < GRID_WIDTH; ++x) {
            if ((y == (GRID_HEIGHT - 1)) && ((x == 0) || (x == (GRID_WIDTH - 1)))) {
                xp += dim;
                xp += grid_offset;

                continue;
            }
#if 0
            u32 coord = LD_CoordToOccupancy(x, y);

            u32 invalid = (play->occupied & play->visual);

            v4 colour = xi_v4_create(0.5, 0.5, 0.5, 1);
            if (invalid & coord) {
                colour = xi_v4_create(1, 0, 0, 1);
            }
            else if (play->occupied & coord) {
                colour = xi_v4_create(0, 1, 0, 1);
            }
            else if (mouse_o & coord) {
                if (__popcnt(mouse_o) < 3) {
                    colour = xi_v4_create(1, 0, 0, 1);
                }
                else {
                    colour = xi_v4_create(1, 1, 0, 1);
                }
            }
            else if (play->visual & coord) {
                colour = xi_v4_create(0, 0, 1, 1);
            }
#endif

            v4 colour = play->grid[(y * GRID_WIDTH) + x];
            xi_quad_draw_xy(renderer, colour, xi_v2_create(xp, yp), xi_v2_create(dim, dim), 0);

            xp += dim;
            xp += grid_offset;
        }

        xp = 0;

        yp += dim;
        yp += grid_offset;
    }

#if 0
    {
        v2 ww = world.xy;
        if (__popcnt(mouse_o) == 3) {
            m2x2 r = xi_m2x2_from_radians(play->angle);
            v2 dd = xi_m2x2_mul_v2(r, xi_v2_create(dim, dim));

            ww = xi_v2_create(dim * mx + (mx * grid_offset) + 0.5f * dd.x, //  (xi_sin(play->angle) * 0.5f * dim),
                              dim * my + (my * grid_offset) + 0.5f * dd.y); // (xi_cos(PI - play->angle) * 0.5f * dim));
        }

        xiImageHandle img = xi_image_get_by_name(&xi->assets, names[0]);
        xi_coloured_sprite_draw_xy_scaled(renderer, img, xi_v4_create(1, 1, 1, 0.5f), ww, 1.0f, play->angle);
    }

    xi_quad_draw_xy(renderer, xi_v4_create(0, 1, 1, 1),
            xi_v2_create(dim * mx + (mx * grid_offset), dim * my + (my * grid_offset)), xi_v2_create(0.2f, 0.2f), 0);
#endif

    xi_quad_draw_xy(renderer, xi_v4_create(1, 0, 1, 1), world.xy, xi_v2_create(0.3f, 0.3f), 0);
}
