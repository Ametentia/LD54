#include "ld_game.h"
#include <stdio.h>

#include "ld_mode_play.c"

extern XI_EXPORT XI_GAME_INIT(xiContext *xi, xi_u32 type) {
    XI_ASSERT(xi->version.major == XI_VERSION_MAJOR);
    XI_ASSERT(xi->version.minor == XI_VERSION_MINOR);
    XI_ASSERT(xi->version.patch == XI_VERSION_PATCH);

    switch (type) {
        case XI_ENGINE_CONFIGURE: {
            xiArena *temp = xi_temp_get();

            xi->window.width  = 1280;
            xi->window.height = 720;
            xi->window.title  = xi_str_wrap_const("game");

            xi->time.delta.fixed_hz = 60;

            xi->system.console_open = true;

            // setup assets
            //
            xiAssetManager *assets = &xi->assets;

            xi_string exe_path = xi->system.executable_path;

            assets->importer.enabled       = true;
            assets->importer.search_dir    = xi_str_format(temp, "%.*s/../assets", xi_str_unpack(exe_path));
            assets->importer.sprite_prefix = xi_str_wrap_const("s_");

            assets->animation_dt = 1.0f / 7.0f;

            assets->sample_buffer.limit = XI_MB(128);

            // setup renderer
            //
            xiRenderer *renderer = &xi->renderer;

            renderer->transfer_queue.limit   = XI_MB(512);

            renderer->sprite_array.dimension = 256;
            renderer->sprite_array.limit     = 256;

            renderer->setup.vsync  = true;
            renderer->layer_offset = 0.01f;

            // setup audio player
            //
            xiAudioPlayer *audio = &xi->audio_player;

            audio->volume = 0.2f;

            audio->music.playing     = true;
            audio->music.volume      = 0.8f;
            audio->music.layer_limit = 16;

            audio->sfx.volume = 0.8f;
            audio->sfx.limit  = 32;
        }
        break;
        case XI_GAME_INIT: {
            // @todo: init game
            //
            xiArena arena = { 0 };
            xi_arena_init_virtual(&arena, XI_GB(8));

            LD_Context *ld = xi_arena_push_array(&arena, LD_Context, 1);
            if (ld) {
                ld->arena = arena;
                ld->xi    = xi;

                xi_arena_init_virtual(&ld->mode_arena, XI_GB(8));

                LD_ModePlayInit(ld);

                xi->user = (void *) ld;
            }
        }
        break;
        case XI_GAME_RELOADED: {
        }
        break;
    }
}

extern XI_EXPORT XI_GAME_SIMULATE(xiContext *xi) {
    LD_Context *ld = (LD_Context *) xi->user;

    switch (ld->mode) {
        case LD_GAME_MODE_PLAY: {
            LD_ModePlayUpdate(ld->play, (f32) xi->time.delta.s);
        }
        break;
    }
}

extern XI_EXPORT XI_GAME_RENDER(xiContext *xi, xiRenderer *renderer) {
    LD_Context *ld = (LD_Context *) xi->user;

    switch (ld->mode) {
        case LD_GAME_MODE_PLAY: {
            LD_ModePlayRender(ld->play, renderer);
        }
        break;
    }

}
