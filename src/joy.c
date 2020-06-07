#include "..\inc\joy.h"

void handleInput(struct player *player, struct gamePlay *game)
{
    u16 value = JOY_readJoypad(JOY_1);

    if (value & BUTTON_LEFT)
    {
        player->properties.xOrder = -1;
        player->buttonsPressed[2] = 1;
        player->buttonsPressed[3] = 0;
        SPR_setHFlip(player->sprite, TRUE);
        
    }
    else if (value & BUTTON_RIGHT) 
    {
        player->properties.xOrder = +1;
        player->buttonsPressed[2] = 0;
        player->buttonsPressed[3] = 1;
        SPR_setHFlip(player->sprite, FALSE);
    }
    else 
    {
        player->properties.xOrder = 0;
    }

    //Check for the START button
    if (value & BUTTON_START)
    {
        //Pause the game
        if(game->gameState == playing)
        {
            game->gameState = pause;
        }
        else if (game->gameState == pause)
        {
            game->gameState = playing;
        }
    }
    //Check for the START button
    if (value & BUTTON_B)
    {
        //game->canKeepJump = TRUE;
    }

    if((value & BUTTON_A) && !player->hasJumped)
    {
        player->hasJumped = TRUE;
        if(!game->jumpTimerStarted)
        {
            //game->canKeepJump = TRUE;;
            game->jumpTimerStarted = TRUE;
            startTimer(game->lastJumpTimer);
        }
    }
    else if(player->hasJumped)
    {
        player->hasJumped = FALSE;
        game->canKeepJump = FALSE;
    }
    
    
}