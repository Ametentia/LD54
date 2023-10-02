typedef u32 LD_ButtonTypes;
enum LD_ButtonTypes
{
    BEGIN,
    EXIT,
    COUNT
};

typedef struct LD_ButtonInfo LD_ButtonInfo;
struct LD_ButtonInfo
{
    const char *asset;
    LD_ButtonTypes type;
};

struct LD_ModeStart
{
    LD_Context *ld;
    xiCameraTransform camera;
    xiArena *arena;

    LD_ButtonInfo buttons[2];
    u32 selectedIndex;
};
