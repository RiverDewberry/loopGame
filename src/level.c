#include "../include/level.h"
#include "../include/raylib.h"
#include <stdlib.h>
#include <stdio.h>

char levelTexturesLoaded = 0;
Texture2D levelTexture;

void LoadLevelTextures()
{
    if(levelTexturesLoaded)return;
    levelTexturesLoaded = 1;
    levelTexture = LoadTexture("./sprites/gameSpriteSheet.png");
}

void UnloadLevelTextures()
{
    if(!levelTexturesLoaded)return;
    levelTexturesLoaded = 0;
    UnloadTexture(levelTexture);
}

Level MakeLevel(char *fileName)
{
    FILE *fptr = fopen(fileName, "rb");

    int width, height, enemyNum;

    fread(&width, 4, 1, fptr);
    fread(&height, 4, 1, fptr);

    char tileTypeGrid[width * height];

    fread(tileTypeGrid, 1, width * height, fptr);
    fread(&enemyNum, 4, 1, fptr);

    int enemyRotations[enemyNum];
    int enemySpawnsX[enemyNum];
    int enemySpawnsY[enemyNum];
    int enemyGoalsX[enemyNum];
    int enemyGoalsY[enemyNum];

    fread(enemyGoalsX, 4, enemyNum, fptr);
    fread(enemyGoalsY, 4, enemyNum, fptr);
    fread(enemySpawnsX, 4, enemyNum, fptr);
    fread(enemySpawnsY, 4, enemyNum, fptr);
    fread(enemyRotations, 4, enemyNum, fptr);

    fclose(fptr);

    LevelTile *tileGrid = malloc(sizeof(LevelTile) * width * height);

    for(int i = 0, index = 0; i < height; i++)
    {
        for(int j = 0; j < width; j++, index++)
        {
            tileGrid[index].type = tileTypeGrid[index];
            if(tileTypeGrid[index] != LEVEL_TILE_WALL)
            {
                tileGrid[index].ySrc = tileTypeGrid[index] << 5;
                tileGrid[index].xSrc = 0;

                if(j - 1 < 0)tileGrid[index].xSrc += 16;
                else if(tileTypeGrid[index - 1] == LEVEL_TILE_WALL)
                    tileGrid[index].xSrc += 16;
                if(j + 1 >= width)tileGrid[index].xSrc += 64;
                else if(tileTypeGrid[index + 1] == LEVEL_TILE_WALL)
                    tileGrid[index].xSrc += 64;
                if(i - 1 < 0)tileGrid[index].ySrc += 16;
                else if(tileTypeGrid[index - width] == LEVEL_TILE_WALL)
                    tileGrid[index].ySrc += 16;
                if(i + 1 >= height)tileGrid[index].xSrc += 32;
                else if(tileTypeGrid[index + width] == LEVEL_TILE_WALL)
                    tileGrid[index].xSrc += 32;
            }
        }
    }

    MacroBot *enemies = (MacroBot*) malloc(
        sizeof(MacroBot) * enemyNum
    );

    for(int i = 0; i < enemyNum; i++) {
        enemies[i] = (MacroBot) {
            .spawnRotation = enemyRotations[i],
            .rotation = enemyRotations[i],
            .spawnX = enemySpawnsX[i],
            .spawnY = enemySpawnsY[i],
            .posX = enemySpawnsX[i],
            .posY = enemySpawnsY[i],
            .goalX = enemyGoalsX[i],
            .goalY = enemyGoalsY[i],
            .curAnimation = MACRO_BOT_ANIMATION_NONE,
            .animationProgress = 0.0f,
            .macro = NULL,
            .macroPos = 0
        };
    }

    enemies[0].macro = (char*) malloc(sizeof(char) * (15 + 1));

    return (Level) {
        .width = width,
        .height = height,
        .tileGrid = tileGrid,
        .enemyNum = 0,
        .maxEnemies = enemyNum - 1,
        .enemies = enemies,
        .player = (MacroBot) {
            .rotation = enemyRotations[0],
            .posX = enemySpawnsX[0],
            .posY = enemySpawnsY[0],
            .goalX = enemyGoalsX[0],
            .goalY = enemyGoalsY[0],
            .spawnX = enemySpawnsX[0],
            .spawnY = enemySpawnsY[0],
            .macro = enemies[0].macro,
            .curAnimation = MACRO_BOT_ANIMATION_NONE,
            .animationProgress = 0.0f,
            .spawnRotation = enemyRotations[0]
        },
        .maxMacroSize = 15
    };

}

void RemoveLevel(Level *lvl)
{
    if(lvl->tileGrid == NULL)
    {
        printf("attempted double free of level\n");
        return;
    }
    free(lvl->tileGrid);
    lvl->tileGrid = NULL;
    for(int i = 0; i < (lvl->maxEnemies + 1); i++)
    {
        if(lvl->enemies[i].macro != NULL)free(lvl->enemies[i].macro);
    }
    if(lvl->enemies == NULL)
    {
        printf("attempted double free of level\n");
        return;
    }
    free(lvl->enemies);
    lvl->enemies = NULL;
}

void DrawEnemySpawn
(
    MacroBot bot,
    int *posX,
    int *posY,
    int frameNum,
    char isEnemy
)
{
        DrawTexturePro(
            levelTexture,
            (Rectangle) {
                32.0f + (float)(16 * (frameNum / 5)), 15.0f, 16.0f, 16.0f
            },
            (Rectangle) {
                (float) posX[bot.spawnX],
                (float) posY[bot.spawnY],
                (float) posX[bot.spawnX + 1] - posX[bot.spawnX],
                (float) posY[bot.spawnY + 1] - posY[bot.spawnY]
            },
            (Vector2) {0.0f, 0.0f},
            0.0f,
            RAYWHITE
        );

        DrawTexturePro(
            levelTexture,
            (Rectangle) {
                80.0f + (float)(16 * (frameNum / 5)), 15.0f, 16.0f, 16.0f
            },
            (Rectangle) {
                (float) posX[bot.goalX],
                (float) posY[bot.goalY],
                (float) posX[bot.goalX + 1] - posX[bot.goalX],
                (float) posY[bot.goalY + 1] - posY[bot.goalY]
            },
            (Vector2) {0.0f, 0.0f},
            0.0f,
            RAYWHITE
        );
}

void DrawBot(
    MacroBot bot,
    int *posX,
    int *posY,
    int frameNum,
    char isEnemy
)
{
    float ySrc = isEnemy ? 15.0f : 0.0f;

    float animationX = 0.0f, animationY = 0.0f, animationTurn = 0.0f;

    float animationSrcMod = 0.0f, animationTransparnecy = 1.0f;

    switch(bot.curAnimation)
    {
        case MACRO_BOT_ANIMATION_NONE:
            break;

        case MACRO_BOT_ANIMATION_WAIT:
            break;

        case MACRO_BOT_ANIMATION_MOVE_UP:
            animationY = -bot.animationProgress;
            break;

        case MACRO_BOT_ANIMATION_MOVE_DOWN:
            animationY = bot.animationProgress;
            break;

        case MACRO_BOT_ANIMATION_MOVE_LEFT:
            animationX = -bot.animationProgress;
            break;

        case MACRO_BOT_ANIMATION_MOVE_RIGHT:
            animationX = bot.animationProgress;
            break;

        case MACRO_BOT_ANIMATION_TURN_RIGHT:
            animationTurn = bot.animationProgress;
            break;

        case MACRO_BOT_ANIMATION_TURN_LEFT:
            animationTurn = -bot.animationProgress;
            break;

        case MACRO_BOT_ANIMATION_TURN_AROUND:
            animationTurn = 2.0f * bot.animationProgress;
            break;

        case MACRO_BOT_ANIMATION_DEAD:
            animationSrcMod = 16.0f;
            animationTransparnecy = 1.0f - bot.animationProgress;
            animationTransparnecy *= 2.0f;
            if(animationTransparnecy > 1.0f)animationTransparnecy = 1.0f;
            break;

        case MACRO_BOT_ANIMATION_GONE:
            animationTransparnecy = 0.0f;
            break;
    }


    int botX = bot.posX;
    int botY = bot.posY;
    float botWidth = (float) posX[botX + 1] - posX[botX];
    float botHeight = (float) posY[botY + 1] - posY[botY];

    DrawTexturePro(
        levelTexture,
        (Rectangle) {
            animationSrcMod,
            ySrc,
            16.0f,
            16.0f
        },
        (Rectangle) {
            (float) posX[botX] + botWidth / 2.0f + botWidth * animationX,
            (float) posY[botY] + botHeight / 2.0f + botHeight * animationY,
            botWidth,
            botHeight
        },
        (Vector2) {
            botWidth / 2.0f,
            botHeight / 2.0f
        },
        90.0f * (bot.rotation + animationTurn),
        (Color) {255, 255, 255, (char)(255.0f * animationTransparnecy)}
    );
}

void DrawLevel(Level lvl, Camera2D cam, int frameNum)
{
    int posX[lvl.width + 1];
    int posY[lvl.height + 1];

    for(int i = 0; i <= lvl.width; i++)
    {
        Vector2 temp = GetWorldToScreen2D(
            (Vector2) {(float)(i << 4), 0.0f}, cam
        );

        posX[i] = (int) temp.x;
    }

    for(int i = 0; i <= lvl.height; i++)
    {
        Vector2 temp = GetWorldToScreen2D(
            (Vector2) {0.0f, (float)(i << 4)}, cam
        );

        posY[i] = (int) temp.y;
    }

    for(int i = 0, index = 0; i < lvl.height; i++)
    {
        for(int j = 0; j < lvl.width; j++)
        {
            if(lvl.tileGrid[index].type != LEVEL_TILE_WALL)
            {
                DrawTexturePro(
                    levelTexture,
                    (Rectangle) {
                        (float) lvl.tileGrid[index].xSrc,
                        (float) lvl.tileGrid[index].ySrc,
                        16.0f, 16.0f
                    },
                    (Rectangle) {
                        (float) posX[j],
                        (float) posY[i],
                        (float) posX[j + 1] - posX[j],
                        (float) posY[i + 1] - posY[i]
                    },
                    (Vector2) {0.0f, 0.0f},
                    0.0f,
                    RAYWHITE
                );
            } else {
                DrawRectangle(
                    posX[j], posY[i],
                    posX[j + 1] - posX[j],
                    posY[i + 1] - posY[i],
                    BLACK
                );
            }

            index++;
        }
    }

    DrawTexturePro(
        levelTexture,
        (Rectangle) {
            32.0f + (float)(16 * (frameNum / 5)), 0.0f, 16.0f, 16.0f
        },
        (Rectangle) {
            (float) posX[lvl.player.spawnX],
            (float) posY[lvl.player.spawnY],
            (float) posX[lvl.player.spawnX + 1] - posX[lvl.player.spawnX],
            (float) posY[lvl.player.spawnY + 1] - posY[lvl.player.spawnY]
        },
        (Vector2) {0.0f, 0.0f},
        0.0f,
        RAYWHITE
    );

    DrawTexturePro(
        levelTexture,
        (Rectangle) {
            80.0f + (float)(16 * (frameNum / 5)), 0.0f, 16.0f, 16.0f
        },
        (Rectangle) {
            (float) posX[lvl.player.goalX],
            (float) posY[lvl.player.goalY],
            (float) posX[lvl.player.goalX + 1] - posX[lvl.player.goalX],
            (float) posY[lvl.player.goalY + 1] - posY[lvl.player.goalY]
        },
        (Vector2) {0.0f, 0.0f},
        0.0f,
        RAYWHITE
    );

    for(int i = 0; i < lvl.enemyNum; i++)
        DrawEnemySpawn(lvl.enemies[i], posX, posY, frameNum, 1);
    for(int i = 0; i < lvl.enemyNum; i++)
        DrawBot(lvl.enemies[i], posX, posY, frameNum, 1);

    DrawBot(lvl.player, posX, posY, frameNum, 0);
}

void RunMacroBot(MacroBot *bot, float animationSpeed)
{
    bot->animationProgress += animationSpeed;
    if(bot->animationProgress >= 1.0f)
    {
        bot->animationProgress = 0.0f;
        switch(bot->curAnimation)
        {
            case MACRO_BOT_ANIMATION_NONE:
                break;

            case MACRO_BOT_ANIMATION_WAIT:
                break;

            case MACRO_BOT_ANIMATION_MOVE_UP:
                bot->posY--;
                break;

            case MACRO_BOT_ANIMATION_MOVE_DOWN:
                bot->posY++;
                break;

            case MACRO_BOT_ANIMATION_MOVE_LEFT:
                bot->posX--;
                break;

            case MACRO_BOT_ANIMATION_MOVE_RIGHT:
                bot->posX++;
                break;

            case MACRO_BOT_ANIMATION_TURN_RIGHT:
                bot->rotation++;
                bot->rotation &= 3;
                break;

            case MACRO_BOT_ANIMATION_TURN_LEFT:
                bot->rotation--;
                bot->rotation &= 3;
                break;

            case MACRO_BOT_ANIMATION_TURN_AROUND:
                bot->rotation += 2;
                bot->rotation &= 3;
                break;

            case MACRO_BOT_ANIMATION_DEAD:
                ResetMacroBot(bot);
                bot->curAnimation = MACRO_BOT_ANIMATION_GONE;
                break;

            case MACRO_BOT_ANIMATION_GONE:
                break;
        }
        if(bot->curAnimation != MACRO_BOT_ANIMATION_GONE)
            bot->curAnimation = MACRO_BOT_ANIMATION_NONE;
    }
}

void ResetMacroBot(MacroBot *bot)
{
    bot->posX = bot->spawnX;
    bot->posY = bot->spawnY;
    bot->macroPos = 0;
    bot->rotation = bot->spawnRotation;
    bot->curAnimation = MACRO_BOT_ANIMATION_NONE;
    bot->animationProgress = 0.0f;
}

// NOTE: updates the macro counter for the bot
MacroBotAnimation GetNextAnimation(MacroBot *bot, Level lvl)
{
    if(bot->macro[bot->macroPos] == 0)return MACRO_BOT_ANIMATION_DEAD;

    int macroPos = bot->macroPos;
    bot->macroPos++;

    if(bot->macro[bot->macroPos] == 0)bot->macroPos = 0;

    MacroBotAnimation retval = MACRO_BOT_ANIMATION_DEAD;

    switch(bot->macro[macroPos])
    {
        case 'W':
            retval = MACRO_BOT_ANIMATION_MOVE_UP;
            break;
        case 'A':
            retval = MACRO_BOT_ANIMATION_MOVE_LEFT;
            break;
        case 'S':
            retval = MACRO_BOT_ANIMATION_MOVE_DOWN;
            break;
        case 'D':
            retval = MACRO_BOT_ANIMATION_MOVE_RIGHT;
            break;

        case 'w':
            retval = (MACRO_BOT_ANIMATION_MOVE_UP + bot->rotation) & 3;
            break;
        case 'a':
            retval = (MACRO_BOT_ANIMATION_MOVE_LEFT + bot->rotation) & 3;
            break;
        case 's':
            retval = (MACRO_BOT_ANIMATION_MOVE_DOWN + bot->rotation) & 3;
            break;
        case 'd':
            retval = (MACRO_BOT_ANIMATION_MOVE_RIGHT + bot->rotation) & 3;
            break;

        //always valid cases
        case 'r':
            return MACRO_BOT_ANIMATION_TURN_RIGHT;
            break;
        case 'l':
            return MACRO_BOT_ANIMATION_TURN_LEFT;
            break;
        case 'u':
            return MACRO_BOT_ANIMATION_TURN_AROUND;
            break;
        case '-':
            return MACRO_BOT_ANIMATION_WAIT;
            break;

        default:
            break;
    }

    int botX = bot->posX;
    int botY = bot->posY;

    switch(retval)
    {
        case MACRO_BOT_ANIMATION_MOVE_UP:
            botY--;
            break;
        case MACRO_BOT_ANIMATION_MOVE_DOWN:
            botY++;
            break;
        case MACRO_BOT_ANIMATION_MOVE_LEFT:
            botX--;
            break;
        case MACRO_BOT_ANIMATION_MOVE_RIGHT:
            botX++;
            break;

        default:
            break;
    }

    if((botX < 0) || (botX >= lvl.width))return MACRO_BOT_ANIMATION_WAIT;
    if((botY < 0) || (botY >= lvl.height))return MACRO_BOT_ANIMATION_WAIT;
    if(lvl.tileGrid[botX + botY * lvl.width].type == LEVEL_TILE_WALL)
        return MACRO_BOT_ANIMATION_WAIT;

    return retval;
}

void UpdateEnemyBotAnimation(Level *lvl)
{
    char resetEnemies = 1;

    for(int i = 0; i < lvl->enemyNum; i++)
        resetEnemies &= (
            lvl->enemies[i].curAnimation == MACRO_BOT_ANIMATION_GONE
        );

    if(resetEnemies)
    {
        for(int i = 0; i < lvl->enemyNum; i++)
            lvl->enemies[i].curAnimation = MACRO_BOT_ANIMATION_NONE;
    }

    for(int i = 0; i < lvl->enemyNum; i++)
    {
        MacroBot *bot = &(lvl->enemies[i]);

        if(bot->curAnimation == MACRO_BOT_ANIMATION_GONE)continue;

        if(
            (bot->posX == bot->goalX) &&
            (bot->posY == bot->goalY)
        )
        {
            bot->curAnimation = MACRO_BOT_ANIMATION_DEAD;
        } else if(
            lvl->tileGrid[
                bot->posX + bot->posY * lvl->width
            ].type == LEVEL_TILE_HAZARD
        )
            bot->curAnimation = MACRO_BOT_ANIMATION_DEAD;
        else bot->curAnimation = GetNextAnimation(bot, *lvl);
    }

    for(int i = 0; i < lvl->enemyNum; i++)
    {
        if(lvl->enemies[i].curAnimation == MACRO_BOT_ANIMATION_GONE)continue;
        if(
            (lvl->enemies[i].posX == lvl->player.posX) &&
            (lvl->enemies[i].posY == lvl->player.posY)
        )
        {
            lvl->player.curAnimation = MACRO_BOT_ANIMATION_DEAD;
            break;
        }
    }
}

void UpdatePlayerBotAnimation(Level *lvl)
{
    if(lvl->player.curAnimation != MACRO_BOT_ANIMATION_NONE)return;
    MacroBot *bot = &(lvl->player);

    if(
        lvl->tileGrid[
            lvl->player.posX + lvl->player.posY * lvl->width
        ].type == LEVEL_TILE_HAZARD
    )
        bot->curAnimation = MACRO_BOT_ANIMATION_DEAD;
    else bot->curAnimation = GetNextAnimation(bot, *lvl);

    for(int i = 0; i < lvl->enemyNum; i++)
    {
        if(lvl->enemies[i].curAnimation == MACRO_BOT_ANIMATION_GONE)continue;
        if(
            (lvl->enemies[i].posX == lvl->player.posX) &&
            (lvl->enemies[i].posY == lvl->player.posY)
        )
        {
            lvl->player.curAnimation = MACRO_BOT_ANIMATION_DEAD;
            break;
        }
    }
}

void UpdateEnemyTiles(Level *lvl)
{
    for(int i = 0; i < lvl->height; i++)
    {
        for(int j = 0; j < lvl->width; j++)
        {
            if(lvl->tileGrid[j + i * lvl->width].type == LEVEL_TILE_PATH)
            {
                lvl->tileGrid[j + i * lvl->width].type = LEVEL_TILE_CLEAR;
                lvl->tileGrid[j + i * lvl->width].ySrc -= 32.0f;
            }
        }
    }

    for(int i = 0; i < lvl->enemyNum; i++)ResetMacroBot(lvl->enemies + i);

    char keepLooping = 0;
    char updateAnimations;

    while(keepLooping)
    {
        keepLooping = 0;
        updateAnimations = 1;
        for(int i = 0; i < lvl->enemyNum; i++)
        {
            keepLooping |= lvl->enemies[i].curAnimation !=
                MACRO_BOT_ANIMATION_GONE;

            updateAnimations &= (lvl->enemies[i].curAnimation ==
                MACRO_BOT_ANIMATION_GONE) || (lvl->enemies[i].curAnimation ==
                MACRO_BOT_ANIMATION_NONE);

            if(
                lvl->tileGrid[
                    lvl->enemies[i].posX + 
                    lvl->enemies[i].posY *
                    lvl->width
                ].type == LEVEL_TILE_CLEAR
            )
            {
                lvl->tileGrid[
                    lvl->enemies[i].posX + 
                    lvl->enemies[i].posY *
                    lvl->width
                ].type = LEVEL_TILE_PATH;

                lvl->tileGrid[
                    lvl->enemies[i].posX + 
                    lvl->enemies[i].posY *
                    lvl->width
                ].ySrc += 32.0f;
            }

            RunMacroBot(lvl->enemies + i, 2.0f);
        }

        if(updateAnimations)UpdateEnemyBotAnimation(lvl);
    }

    ResetMacroBot(&(lvl->player));

    for(int i = 0; i < lvl->enemyNum; i++)ResetMacroBot(lvl->enemies + i);
}

void ResetLevel(Level *lvl)
{
    for(int i = 0; i < lvl->maxEnemies; i++)
    {
        lvl->enemies[i].macroPos = 0;
        if(lvl->enemies[i].macro != NULL)
            *(lvl->enemies[i].macro) = 0;

        lvl->enemies[i].posX = lvl->enemies[i].spawnX; 
        lvl->enemies[i].posY = lvl->enemies[i].spawnY; 
        lvl->enemies[i].rotation = lvl->enemies[i].spawnRotation;
    }
    lvl->player = lvl->enemies[0];
    lvl->enemyNum = 0;
}
