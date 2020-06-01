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


unsigned int palette0[128] = {
	0x0666,0x0222,0x0442,0x000E,0x0EE0,0x00E0,0x0E00,0x0888,	//  Pallette: 0
	0x0CCC,0x0088,0x0808,0x0008,0x0880,0x0080,0x0800,0x0000,	
	} ;
