#include <genesis.h>

#define JUMP_SPEED_MIN      FIX32(4L)
#define JUMP_SPEED_MAX      FIX32(15L)
#define JUMP_SPEED_DEFAULT  FIX32(7.8L)


enum gameStates {
    menu,
    playing,
    pause,
    dead,
    complete
};

enum enemyTypes {
    enemy1,
    enemy2,
    enemy3
};

struct object
{
    fix32 x, y;
    fix32 velX, velY;
    fix32 maxSpeed, jumpSpeed, speed, min;
    int xOrder;
};

typedef struct player
{
    int health;
    int energy;
    struct object properties;
    Sprite* sprite;
    int buttonsPressed[7];
    bool onGround, jumping, falling, hasJumped;
    int tileX, tileY;
}player;

struct enemy
{
    int health;
    int energy;
    int attackStrength;
    struct object properties;
    int enemyType;
    Sprite* sprite;
};

struct gamePlay
{
    int timer;
    int score;
    int gameState;
    fix32 prevDeathLocation;
    unsigned int** currentLevelData;
    u16 lastJumpTimer;
    bool jumpTimerStarted, canKeepJump;
    Sprite *debugTileBottom, *debugTileLeft, *debugTileRight;
};