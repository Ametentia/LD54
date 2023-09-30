#if !defined(LD_GAME_H_)
#define LD_GAME_H_

#include <xi/xi.h>

#define internal static
#define function static
#define local static
#define global static

typedef xi_u8  u8;
typedef xi_u16 u16;
typedef xi_u32 u32;
typedef xi_u64 u64;

typedef xi_s8  s8;
typedef xi_s16 s16;
typedef xi_s32 s32;
typedef xi_s64 s64;

typedef xi_s8  b8;
typedef xi_s16 b16;
typedef xi_s32 b32;
typedef xi_s64 b64;

typedef xi_f32 f32;
typedef xi_f64 f64;

typedef xi_uptr uptr;
typedef xi_sptr sptr;

typedef xi_v2u v2u;
typedef xi_v2s v2s;

typedef xi_v2 v2;
typedef xi_v3 v3;
typedef xi_v4 v4;

typedef xi_rect2 rect2;
typedef xi_rect3 rect3;

typedef xi_m2x2 m2x2;
typedef xi_m4x4 m4x4;

typedef struct LD_ModePlay LD_ModePlay;

typedef u32 LD_GameMode;
enum LD_GameMode {
    LD_GAME_MODE_NONE = 0,
    LD_GAME_MODE_PLAY
};

typedef struct LD_Context LD_Context;
struct LD_Context {
    xiContext *xi;

    xiArena arena;
    xiArena mode_arena;

    LD_GameMode mode;
    union {
        LD_ModePlay *play;
    };
};

#endif  // LD_GAME_H_
