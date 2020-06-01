//#include "enemy.h"
#include <genesis.h>
#include "object.h"

//This moves a sprite on screen
//It also hides a sprite when its out of the viewable area.
static void setSpritePosition(Sprite* sprite, s16 posX, s16 posY);
static void joyEvent(u16 joy, u16 changed, u16 state);
void movePlayer(struct player *player, struct gamePlay *game);
bool checkFloor(struct player *player, struct gamePlay *game);
bool checkCeiling(struct player *player);
bool checkRight(struct player *player);
bool checkLeft(struct player *player);
void playerJump(struct player *player);
static void updateCamera(struct player *player);
static void debugInfo();