/**
 * Hello World Example
 * Created With Genesis-Code extension for Visual Studio Code
 * Use "Genesis Code: Compile" command to compile this program.
 **/
#include "main.h"
#include "sprite.h"
#include "gfx.h"
#include "kdebug.h"
#include "temp.h"
#include "bmp.h"

/*The edges of the play field*/
const int LEFT_EDGE = 0;
const int RIGHT_EDGE = 320;
const int TOP_EDGE = 0;
const int BOTTOM_EDGE = 224;

static void updateCamera(struct player *player);
static void debugInfo();
    
s16 currentPixels = 0;       //Detect current pixels moved, if it hits 8, reset 
s16 offset = 0;             //scroll amount in pixels
s16 column_to_update = 0;   //column to update while scrolling
s16 columnOffset = 40;      //offset to be used in our collision map


TileMap *testMap; //Map DATA

int main()
{
    
    char buffer[20];

    // disable interrupt when accessing VDP
    SYS_disableInts();
    // initialization
    VDP_init();
    VDP_setScreenWidth320();
    VDP_setScreenHeight224();
    // Set up VDP modes
    VDP_setHilightShadow(0);
    VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);

    VDP_setPlanSize(64, 32);
     //Setup our joypads
    JOY_init();
    JOY_setEventHandler(NULL);

    // reduce DMA buffer size to avoid running out of memory (we don't need it)
    //Enable sys interrupts
    SYS_enableInts();

    //lets draw out shitty map
    //VDP_loadTileSet(level1.tileset, 0, DMA);
    VDP_loadTileSet(bgTile.tileset, 0, DMA );
    VDP_loadTileSet(bgTile.tileset, 0, DMA );
    VDP_loadTileSet(bgTile.tileset, 0, DMA );

    //VDP_setMap(BG_B, &testMap, 0, 80, 28);
    //VDP_setTileMapEx(BG_B, &testMap, TILE_ATTR_FULL(PAL0, 0, FALSE, FALSE, TILE_USERINDEX), 0, 0, 0, 0, 80, 28, DMA);

    VDP_setPalette(PAL0, bgTile.palette->data);
    VDP_setBackgroundColor(0xFF);

    int tile =0;

    //testMap = unpackTileMap(&mapData, NULL);

    testMap = allocateTileMapEx(80, 28);
    testMap->tilemap = &mapData;
    


    
    // for(int y=0; y < 28; y++ )
    // {
    //     for (int x = 0; x < 40; x++)
    //     {
    //         if(mapData[tile] >= 0x1)
    //         {
    //             VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, mapData[tile]), x, y);
                
    //         }
            
    //         tile++;
    //     }
    // }
    

   VDP_setTileMapEx(BG_B, testMap, TILE_ATTR_FULL(PAL0, 0, FALSE, FALSE, mapData[TILE_USERINDEX]-1), 0,0, 0,0,40,28, DMA_QUEUE);
    
    
   /*
    for(int y=0; y < 28; y++ )
    {
        for (int x = 0; x < 40; x++)
        {
            if(mapData[tile] >= 0x1)
            {
                VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, mapData[tile]-1), x, y);
                
            }
            
            tile++;
        }
    }
*/

    //allocateTileMapEx()


    //VDP_drawImageEx(BG_B, &bg_image, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
    //ind += bg_image.tileset->numTile;

    //Lets setup our Palettes here
    VDP_setPalette(PAL3, playerSprite.palette->data);
    VDP_setTextPalette(PAL1);

     // init sprite engine with default parameters
    SPR_init(0,0,0);

    

    player myPlayer;

    myPlayer.properties.velX = FIX32(0L);
    myPlayer.properties.velY = FIX32(0L);
    myPlayer.properties.maxSpeed = FIX32(20L);
    myPlayer.properties.min = FIX32(1L);
    myPlayer.properties.x = intToFix32(10);
    myPlayer.properties.y = intToFix32(10);
    myPlayer.properties.maxSpeed = FIX32(3L);
    myPlayer.onGround = FALSE;
    myPlayer.falling = FALSE;
    myPlayer.jumping = FALSE;
    myPlayer.hasJumped = FALSE;
    myPlayer.sprite = SPR_addSprite(&playerSprite, 100, 100, TILE_ATTR(PAL3,0, FALSE, FALSE));

    //Define our gameplay object, tracks scores, gamestate etc
    struct gamePlay game;
    game.timer = 100;
    game.score = 0;
    game.gameState = playing;
    game.lastJumpTimer = 0;
    const int scrollspeed = 2; 

    game.debugTileBottom = SPR_addSprite(&debugSprite, 100, 100, TILE_ATTR(PAL3,0, FALSE, FALSE));
    game.debugTileLeft = SPR_addSprite(&debugSprite, 100, 100, TILE_ATTR(PAL2,0, FALSE, FALSE));


    while(1)
    {

        if(game.canKeepJump == TRUE)
        {
            game.canKeepJump = FALSE;
            testMap->tilemap = &level2;
        }

        // update sprites
        movePlayer(&myPlayer, &game);
        handleInput(&myPlayer, &game);
        SPR_update();
        //Wait for VSync
        VDP_waitVSync();
        updateCamera(&myPlayer);
        debugInfo();
        //VDP_drawText("                                 ", 10, 10);
        
    }
    return (0);
}

bool checkFloor(struct player *player, struct gamePlay *game)
{
    char buffer[40];
    int tilex, tiley, currentTileIndex;;
    //Calculate which tilemap the player is interacting with
    //Adjusts for scrolling of the map
    if(offset != 0 || offset !=(testMap->w<<2))
    {
        tilex = (fix32ToInt(player->properties.x)+8) >> 3;
    }
    else
    {
        tilex = (fix32ToInt(player->properties.x)) >> 3;
    }
    
    //Since we dont scroll vertically in this game... this is acceptable
    tiley = (fix32ToInt(player->properties.y) + 16)/8;  

    
    currentTileIndex = (columnOffset - 40) + tilex + (testMap->w * tiley);
 
    if(testMap->tilemap[currentTileIndex]!=NULL)
    {
        char buff[20];
        sprintf(buff, "TileID: %d", testMap->tilemap[currentTileIndex]);
        VDP_drawText("                    ", 10, 13);
        VDP_drawText(buff, 10, 13);
        setSpritePosition(game->debugTileBottom, tilex << 3, tiley<<3);
        return TRUE;
    }
    
    MEM_free(buffer);
    
/*
    tilex = (x)/8;
    currentTileIndex = tilex + (40 * tiley);
    if(mapData[currentTileIndex] != 0)
    {
        //pushplayer UP
      //  if((tempY - 16) != y)
       // player->properties.y = intToFix32(tempY-16);
       setSpritePosition(game->debugTile, x, y);
        return TRUE;
    }

    tilex = (x + 15)/8;
    currentTileIndex = tilex + (40 * tiley);
    if(mapData[currentTileIndex] != 0)
    {
        //pushplayer UP
      //  if((tempY - 16) != y)
       //     player->properties.y = intToFix32(tempY-16);
       setSpritePosition(game->debugTile, x, y);
        return TRUE;
    }
*/
    return FALSE;
}
bool checkCeiling(struct player *player)
{
    //Calculate which tilemap the player is interacting with
    int x, y, tilex, tiley;
    int tempY;
    x = fix32ToInt(player->properties.x);
    y = fix32ToInt(player->properties.y);


    tilex = (x + 8 )/8;
    tiley = (y)/8;

    tempY = tiley * 8;

    int currentTileIndex = tilex + (40 * tiley);
    if(mapData[currentTileIndex] != 0)
        return TRUE;
    

    tilex = (x)/8;
    currentTileIndex = tilex + (40 * tiley);
    if(mapData[currentTileIndex] != 0)
            return TRUE;

    tilex = (x + 16)/8;
    currentTileIndex = tilex + (40 * tiley);
    if(mapData[currentTileIndex] != 0)
    {
        
         return TRUE;
    }

    return FALSE;
}
bool checkLeft(struct player *player)
{
    
    //Calculate which tilemap the player is interacting with
    int x, y, tilex, tiley;
    x = fix32ToInt(player->properties.x);
    y = fix32ToInt(player->properties.y);

    
    tilex = (x)/8;
    tiley = (y - 8)/8;

    int currentTileIndex = tilex + (40 * tiley);
    if(mapData[currentTileIndex] != 0)
    {
         //push player onto tile
        int tempX = tilex * 8;  
        if((tempX + 16) != x)
        {
        player->properties.x = intToFix32(tempX+8);
        }
        
        return TRUE;
    }

    tiley = (y - 15)/8;
    
    currentTileIndex = tilex + (40 * tiley);
    if(mapData[currentTileIndex] != 0)
        return TRUE;

    tiley = (y)/8;
    currentTileIndex = tilex + (40 * tiley);
    if(mapData[currentTileIndex] != 0)
        return TRUE;

        return FALSE;
    
}
bool checkRight(struct player *player)
{
    char buffer[40];
    //Calculate which tilemap the player is interacting with
    int x, y, tilex, tiley;
    x = fix32ToInt(player->properties.x);
    y = fix32ToInt(player->properties.y);


    tilex = (x + 15)/8;
    tiley = (y - 8)/8;

    int currentTileIndex = tilex + (40 * tiley);
    if(mapData[currentTileIndex] != 0)
    {
         //push player onto tile
        int tempX = tilex * 8;  
        //sprintf (buffer, "X:  '%d  tx:  %d", x, tempX);
        KLog_U2("X: ", x, "X2: ", tempX);

        //VDP_drawText(buffer, 10, 10);
        
        if((tempX) != x)
        player->properties.x = intToFix32(tempX - 16);
        
        return TRUE;
    }

    tiley = (y - 16)/8;
    currentTileIndex = tilex + (40 * tiley);
    if(mapData[currentTileIndex] != 0)
        return TRUE;

    tiley = (y)/8;
    currentTileIndex = tilex + (40 * tiley);
    if(mapData[currentTileIndex] != 0)
        return TRUE;

        return FALSE;
    
}

void movePlayer(struct player *player, struct gamePlay *game)
{
    bool canMoveLeft = TRUE;
    bool canMoveRight = TRUE;
    if(game->gameState == playing)
    {
            //Accelerate to the RIGHT
        if(player->properties.xOrder == 1 && canMoveRight)
        {
            //offset++; 
            //currentPixels++;  
            //if(currentPixels>7) currentPixels=0;  //del 0 al 7 son 8px=1 TILE
            player->properties.velX += FIX32(0.1L);
            //player->properties.x += intToFix32(1);
            //offset+= fix32ToInt(FIX32(0.1L));
            if(player->properties.velX > player->properties.maxSpeed)
            {
                player->properties.velX = player->properties.maxSpeed;
            }
        }

        //Accelerate to the LEFT
        else if(player->properties.xOrder == -1 && canMoveLeft)
        {
            //offset--; 
            //currentPixels--;  
            //if(currentPixels<-7) currentPixels=0;

            player->properties.velX -= FIX32(0.1L);
            //player->properties.x -= intToFix32(1);
            //offset-= fix32ToInt(FIX32(0.1L));
            if(player->properties.velX < -player->properties.maxSpeed)
            {
                player->properties.velX = -player->properties.maxSpeed;
            }
        }

        else
        {
        if ((player->properties.velX < FIX32(0.1)) && (player->properties.velX > FIX32(-0.1)))
                player->properties.velX = 0;

            //Decellerate player that was going RIGHT
            if(player->properties.velX >= FIX32(0L))
            {
                player->properties.velX -= FIX32(0.3L);
                if(player->properties.velX < FIX32(0L))
                player->properties.velX = 0;
            }

            //Decellerate player that was going LEFT
            if(player->properties.velX <= FIX32(0L))
            {
                player->properties.velX += FIX32(0.3L);
                if(player->properties.velX > FIX32(0L))
                player->properties.velX = 0;
            }
            
        }
        if(player->hasJumped && !checkCeiling(player) )
        {
            if(getTimer(game->lastJumpTimer, 0) < 30000 && game->canKeepJump )
            {
                player->properties.velY -= FIX32(0.7L);
                //Clamp our max speed
                if(player->properties.velY < FIX32(-3L))
                {
                    game->canKeepJump = FALSE;
                    player->properties.velY = FIX32(-3L);
                }
            }
            else
            {
                game->canKeepJump = FALSE;
                //player->properties.velY = 0;
                game->jumpTimerStarted = FALSE;
                player->hasJumped = FALSE;
            }
        }

        //Simulate gravity in our game
        if(!checkFloor(player, game) && canMoveRight){
                player->falling = TRUE;
                player->properties.velY += FIX32(0.1L);
                if(player->properties.velY > FIX32(2L))
                {
                    player->properties.velY = FIX32(2L);
                }
                player->onGround = FALSE;
        }
        else if(!player->hasJumped)
        {
            //push player onto tile
            int y = fix32ToInt(player->properties.y);
            int tiley = (y + 16)/8;
            int tempY = tiley * 8;  
            if((tempY - 16) != y)
            player->properties.y = intToFix32(tempY-16);

            //set our velocity to 0 (we are on the ground)
            player->properties.velY = 0;
            player->onGround = TRUE;
            player->falling = FALSE;
        }

        //Set our x and y position on screen
        player->properties.x += player->properties.velX;
        player->properties.y += player->properties.velY;


            offset = offset + fix32ToInt(player->properties.velX);
            currentPixels = currentPixels + fix32ToInt(player->properties.velX);
            if(currentPixels > 1) currentPixels=0;
            if(currentPixels < -1) currentPixels=-1;
            //if(currentPixels<-15) currentPixels=0;
        

        
        //Show our X and Y position
        char buffer[20];
        sprintf(buffer, "X:%d Y:%d", fix32ToInt(player->properties.x), fix32ToInt(player->properties.y));
        VDP_drawText("                    ", 1, 2);
        VDP_drawText(buffer, 1,2);

        //Show our tile position
        int tilex, tiley;
        tilex = (fix32ToInt(player->properties.x) >> 3);
        tiley = (fix32ToInt(player->properties.y) >> 3);

        sprintf(buffer, "X:%d Y:%d", tilex, tiley);
        VDP_drawText("                    ", 1, 3);
        VDP_drawText(buffer, 1,3);

        sprintf(buffer, "Vel: %d", fix32ToInt(player->properties.velX));
        VDP_drawText("                    ", 1, 4);
        VDP_drawText(buffer, 1,4);
        MEM_free(buffer);
        
        //Basic "deadzone" before movement will occur
        if(tilex < 13 && offset != 0)
        {
            // offset--; 
            // currentPixels--;  
            // if(currentPixels<-7) currentPixels=0;
           // player->properties.x = intToFix32(13 << 3);
            
        }
        //Basic "deadzone" before movement will occur
        else if(tilex > 22 && offset != (testMap->w << 2))
        {
            // offset++; 
            // currentPixels++;  
            // if(currentPixels>7) currentPixels=0;
            //player->properties.x = intToFix32(23 << 3)-1; //-1 so that the scrolling doesnt keep happening when we release right
        }

        //Yep Alfred sure is slow
        setSpritePosition(player->sprite, fix32ToInt(player->properties.x), fix32ToInt(player->properties.y));
    }
}

static void setSpritePosition(Sprite* sprite, s16 x, s16 y)
{
    // clip out of screen sprites
    if ((x < -100) || (x > 320) || (y < -100) || (y > 240)) SPR_setVisibility(sprite, HIDDEN);
    else
    {
        SPR_setVisibility(sprite, VISIBLE);
        SPR_setPosition(sprite, x, y);
    }
}
static void joyEvent(u16 joy, u16 changed, u16 state)
{
    /*
    // START button state changed --> pause / unpause
    if (changed & state & BUTTON_START)
    {
        paused = !paused;
        updateBarsVisitility();
    }

    // can't do more in paused state
    if (paused) return;

    if (changed & state & (BUTTON_A | BUTTON_B | BUTTON_C))
    {
        if (movY == 0)
        {
            movY = -jumpSpeed;
            SND_startPlayPCM_XGM(SFX_JUMP, 1, SOUND_PCM_CH2);
        }
    }
    */
}

static void updateCamera(struct player *player)
{
    //Check if we are at the start of the level
    if(offset < 0) offset=0;
    //Weve hit the end of the level, stop scrolling
    if(offset > (testMap->w <<2)) offset = (testMap->w<<2);

    //Offset that determines where to start counting for the collision map (currently just uses the currently loaded)
    columnOffset = (((offset + 320) >>3)) & 127;


    //Move to the right
    if(currentPixels==0)
    {
        column_to_update = (((offset + 320) >>3)) & 127;

        VDP_setTileMapEx(BG_B, testMap, TILE_ATTR_FULL(PAL0, 0, FALSE, FALSE, 0), column_to_update, 0, column_to_update, 0, 1, 28, DMA_QUEUE );
        
    }
    //Move to the left
    if(currentPixels==-1)
    {
        column_to_update = (((offset + 320) >> 3) + 88) & 127;
       VDP_setTileMapEx(BG_B, testMap, TILE_ATTR_FULL(PAL0, 0, FALSE, FALSE, 0), column_to_update, 0, column_to_update, 0, 1, 28, DMA_QUEUE );
    }

    //hacemos scroll
    VDP_setHorizontalScroll(BG_B, -offset);

}

static void debugInfo()
{
    char buffer[20];
    sprintf(buffer, "Offset: %d", offset);
    VDP_drawText("                    ", 10, 10);
    VDP_drawText(buffer, 10, 10);
    sprintf(buffer, "Col: %d", columnOffset);
    VDP_drawText("                    ", 10, 12);
    VDP_drawText(buffer, 10, 12);
    MEM_free(buffer);
}