#include "ld_mode_start.h"

function void LD_ModeStartInit(LD_Context *ld)
{
    xi_arena_reset(&ld->mode_arena);

    xiContext *xi = ld->xi;
    LD_ModeStart *start = xi_arena_push_array(&ld->mode_arena, LD_ModeStart, 1);
    start->ld = ld;
    start->arena = &ld->arena;
    ld->start = start;
    start->selectedIndex = 0;

    f32 aspect = (xi->window.width / (f32)xi->window.height);
    xi_camera_transform_get_from_axes(&start->camera, aspect,
                                      xi_v3_create(1, 0, 0), xi_v3_create(0, 1, 0), xi_v3_create(0, 0, 1), xi_v3_create(0, 0, 5), 0);

    start->buttons[0].type = 0;
    start->buttons[0].asset = "play";

    start->buttons[1].type = 1;
    start->buttons[1].asset = "exit";

    xiSoundHandle music = xi_sound_get_by_name(&xi->assets, "menu_pipa");
    XI_ASSERT(music.value != 0);
    xi_music_layer_add(&xi->audio_player, music, 0);
    xi_music_layer_enable_by_index(&xi->audio_player, 0, XI_MUSIC_LAYER_EFFECT_FADE, 1.0f);
}

function void LD_ModeStartUpdate(LD_ModeStart *start, f32 dt)
{
    LD_Context *ld = start->ld;
    xiContext *xi = ld->xi;

    xiInputKeyboard *kb = &xi->keyboard;
    if (kb->keys[' '].pressed || kb->keys['e'].pressed || kb->keys['f'].pressed || kb->keys[XI_KEYBOARD_KEY_RETURN].pressed)
    {
        if (start->selectedIndex == BEGIN)
        {
            start->ld->mode = LD_GAME_MODE_PLAY;
        }
        if (start->selectedIndex == EXIT)
        {
            xi->quit = true;
        }
    }
    if (kb->keys[XI_KEYBOARD_KEY_UP].pressed || kb->keys[XI_KEYBOARD_KEY_DOWN].pressed || kb->keys[XI_KEYBOARD_KEY_W].pressed || kb->keys[XI_KEYBOARD_KEY_S].pressed)
    {
        start->selectedIndex = 1 - start->selectedIndex;
    }
}

function void LD_ModeStartRender(LD_ModeStart *start, xiRenderer *renderer)
{
    LD_Context *ld = start->ld;
    xiContext *xi = ld->xi;

    xi_camera_transform_set_from_axes(renderer, xi_v3_create(1, 0, 0), xi_v3_create(0, 1, 0),
                                      xi_v3_create(0, 0, 1), xi_v3_create(0, 0, 5), 0);

    rect3 bounds = xi_camera_bounds_get(&renderer->camera);
    v2 screen_centre = xi_v2_mul_f32(xi_v2_add(bounds.max.xy, bounds.min.xy), 0.5f);
    v2 screen_dim = xi_v2_sub(bounds.max.xy, bounds.min.xy);
    {
        xiImageHandle image = xi_image_get_by_name(&xi->assets, "title_background");

        xi_quad_draw_xy(renderer, xi_v4_create(0.03f, 0.004f, 0.001f, 1), screen_centre, xi_v2_create(screen_dim.x, screen_dim.y), 0);

        xi_sprite_draw_xy_scaled(renderer, image, screen_centre, 11.3f, 0);
    }
    xiImageHandle image = xi_image_get_by_name(&xi->assets, "play_arrow");
    xi_sprite_draw_xy_scaled(renderer, image, xi_v2_create(screen_centre.x - 0.8f, (screen_centre.y - 1) - 1.2f * start->selectedIndex), 0.35f, 0);
    for (u32 i = 0; i < COUNT; i++)
    {
        xiImageHandle play_image = xi_image_get_by_name(&xi->assets, start->buttons[i].asset);
        xi_sprite_draw_xy_scaled(renderer, play_image, xi_v2_create(screen_centre.x, (screen_centre.y - 1) - 1.2f * i), 1.0f, 0);
    }
}