#ifndef LEVEL_H
#define LEVEL_H

#include <raylib.h>

extern char levelTexturesLoaded;

void LoadLevelTextures();
void UnloadLevelTextures();

typedef enum LevelTileType {
    //a wall
    LEVEL_TILE_WALL = 0,

    //empty space
    LEVEL_TILE_CLEAR = 1,

    //an empty space in the path of an enemy
    LEVEL_TILE_PATH = 2,

    //a hazard
    LEVEL_TILE_HAZARD = 3
} LevelTileType;

typedef struct LevelTile {
    //the type of the tile
    LevelTileType type;

    //the x position of the sprite for the tile on the sprite sheet
    short int xSrc;

    //the y position of the sprite for the tile on the sprite sheet
    short int ySrc;
} LevelTile;

typedef enum MacroBotAnimation {
    //no animation
    MACRO_BOT_ANIMATION_NONE = 4,

    //waiting
    MACRO_BOT_ANIMATION_WAIT = 5,

    //moving up
    MACRO_BOT_ANIMATION_MOVE_UP = 0,

    //moving down
    MACRO_BOT_ANIMATION_MOVE_DOWN = 2,

    //moving left
    MACRO_BOT_ANIMATION_MOVE_LEFT = 3,

    //moving right
    MACRO_BOT_ANIMATION_MOVE_RIGHT = 1,

    //turning right
    MACRO_BOT_ANIMATION_TURN_RIGHT = 6,

    //turning left
    MACRO_BOT_ANIMATION_TURN_LEFT = 7,

    //turning around
    MACRO_BOT_ANIMATION_TURN_AROUND = 8,

    //if the bot is dead
    MACRO_BOT_ANIMATION_DEAD = 9,

    //if the bot is dead and waiting to be respawned
    MACRO_BOT_ANIMATION_GONE = 10
} MacroBotAnimation;

typedef struct MacroBot {
    //the rotation of the bot
    char rotation;

    //the initial rotation of the bot
    char spawnRotation;

    //the x position of the bot
    int posX;

    //the y position of the bot
    int posY;

    //the x position of the spawn of the bot
    int spawnX;

    //the y position of the spawn of the bot
    int spawnY;

    //the x position of the goal of the bot
    int goalX;

    //the y position of the goal of the bot
    int goalY;

    //where in the macro the bot is
    int macroPos;

    //the macro that controls the bot
    char *macro;

    //the animation of the bot
    MacroBotAnimation curAnimation;

    //the animation progress as a value from 0.0f (no progress) to 1.0f (finished)
    float animationProgress;
} MacroBot;

void RunMacroBot(MacroBot *bot, float animationSpeed);


//resets a macro bot
void ResetMacroBot(MacroBot *bot);

typedef struct Level {
    //the width of the level
    int width;

    //the height of the level
    int height;

    //the grid of tiles for the level
    LevelTile *tileGrid;

    //the max number of enemies
    int maxEnemies;

    //the number of enemy bots
    int enemyNum;

    //the enemies
    MacroBot *enemies;

    //the player
    MacroBot player;

    //maximum macro size for the player
    char maxMacroSize;
} Level;

//makes a Level struct, does alloc some mem, and thus needs to be free'd
Level MakeLevel(
    int width,
    int height,
    LevelTileType *tileTypeGrid,
    int enemyNum,
    int *enemySpawnsX,
    int *enemySpawnsY,
    int *enemyGoalsX,
    int *enemyGoalsY,
    int *enemyRotations,
    char **enemyMacros,
    int playerSpawnX,
    int playerSpawnY,
    int playerGoalX,
    int playerGoalY,
    char playerStartRotation,
    char maxMacroSize

);

//frees a level
void RemoveLevel(Level *lvl);

//draws a level
void DrawLevel(Level lvl, Camera2D cam, int frameNum);

//Updates the animation of enemy bots on a level
void UpdateEnemyBotAnimation(Level *lvl);

//Updates the animation of the player bot on a level
void UpdatePlayerBotAnimation(Level *lvl);

#endif
