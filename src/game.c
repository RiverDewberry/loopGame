#include <raylib.h>
#include <stdio.h>
#include <string.h>
#include "../include/rayui.h"
#include "../include/level.h"

int main(int argc, char **argv)
{
    InitWindow(500, 500, "MACRO - by River Dewberry");

    ChangeDirectory(GetDirectoryPath(*argv));
    printf("%s\n", GetDirectoryPath(*argv));
    LoadLevelTextures();

    Level lvl1 = MakeLevel(
        9, 4,
        (LevelTileType[]) {
            2, 2, 2, 2, 2, 0, 0, 0, 0,
            1, 3, 2, 2, 2, 3, 1, 1, 1,
            1, 0, 1, 0, 1, 0, 1, 0, 1,
            1, 1, 1, 3, 1, 1, 1, 3, 1,
            0, 0, 2, 2, 2, 2, 2, 2, 0
        },
        1,
        (int[]) {
            0
        },
        (int[]) {
            0
        },
        (int[]) {
            4
        },
        (int[]) {
            0
        },
        (int[]) {
            1
        },
        (char*[]) {
            "wwDr"
        },
        0, 1, 8, 3, 2, 15
    );

    Level curLvl = lvl1;


    Camera2D worldCam = (Camera2D) {
        .target = (Vector2) {
            8.0f * lvl1.width,
            8.0f * lvl1.height
        },
        .zoom = 1.0f,
        .rotation = 0.0f,
        .offset = (Vector2) {GetScreenWidth()/2.0f, GetScreenHeight()/2.0f}
    };

    float camDragScale = -1.0f;

    Dragger worldCamDrag = MakeDragger(
        DRAGGER_NO_HITBOX,
        (union DraggerHitbox) {},
        DRAGGER_VECTORS_TARGET,
        (union DraggerTarget) {.vecs = {.pos = &worldCam.target, .dim = NULL}},
        DRAGGER_REC_BOUNDS,
        (union DraggerBounds) {.rec = (Rectangle[]) {
            0.0f,
            0.0f,
            (float)(lvl1.width * 16),
            (float)(lvl1.height * 16)
        }},
        DRAGGER_BOUND_BOX,
        &camDragScale,
        &worldCam
    );

    float uiScale = 1.0f;

    Element topBar = MakeBox(
        (Color) {0x1e, 0x1e, 0x1e, 0xc0},
        (Vector2) {0.0f, 0.0f},
        (Vector2) {(float) GetScreenWidth(), 50.0f},
        &uiScale
    );

    float speed = 0.05f;

    SetTargetFPS(30);

    int frameCtr = 0;

    char playerDead = 0;
    char playerTurn = 0;

    strcpy(curLvl.player.macro, "wwDDu------");

    while (!WindowShouldClose())
    {
        ClearBackground(BLACK);

        uiScale = ((float) GetScreenWidth()) / 500.0f;

        Zoom(&(worldCam.zoom), 1.0f, 64.0f, 0.125f);
        CenterCamera(&worldCam);
        worldCamDrag.clicked = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
        UpdateDrag(&worldCamDrag);

        DrawLevel(curLvl, worldCam, frameCtr);

        if(
            (curLvl.player.posX == curLvl.player.goalX) &&
            (curLvl.player.posY == curLvl.player.goalY)
        )
        {

        } else if(curLvl.enemyNum == 0)
        {
            RunMacroBot(&(curLvl.player), speed);
            if(curLvl.player.curAnimation == MACRO_BOT_ANIMATION_NONE)
                UpdatePlayerBotAnimation(&curLvl);
        } else {
            if(playerTurn)
            {
                RunMacroBot(&(curLvl.player), speed);

                if(curLvl.player.curAnimation == MACRO_BOT_ANIMATION_NONE)
                {
                    playerTurn = 0;

                    if(playerDead)
                    {
                        for(int i = 0; i < curLvl.enemyNum; i++)
                            curLvl.enemies[i].curAnimation =
                                MACRO_BOT_ANIMATION_DEAD;
                    } else UpdateEnemyBotAnimation(&curLvl);
                }
            } else {
                playerTurn = 1;
                for(int i = 0; i < curLvl.enemyNum; i++)
                {
                    RunMacroBot(curLvl.enemies + i, speed);
                    playerTurn &= curLvl.enemies[i].curAnimation ==
                        MACRO_BOT_ANIMATION_NONE;
                }

                playerDead = 0;

                if(playerTurn)
                {
                    UpdatePlayerBotAnimation(&curLvl);
                    playerDead = curLvl.player.curAnimation ==
                        MACRO_BOT_ANIMATION_DEAD;
                }
            }
        }

        DrawElement(topBar);
        int macroFontSize = uiScale * 10;
        int drawStart = (
            GetScreenWidth() - (macroFontSize * curLvl.maxMacroSize * 2)
        ) / 2;
        char macroLen = strlen(curLvl.player.macro);
        char tmpStr[] = {0, 0};
        for(int i = 0; i < macroLen; i++)
        {
            tmpStr[0] = curLvl.player.macro[i];
            DrawText(
                tmpStr,
                drawStart + macroFontSize * i * 2,
                30 * uiScale,
                macroFontSize,
                (
                    (((i + 1) % macroLen) == curLvl.player.macroPos) &&
                    (curLvl.player.curAnimation != MACRO_BOT_ANIMATION_NONE)
                ) ?
                    RED : WHITE
            );

            if(!(i + 1 < macroLen))break;

            DrawText(
                ",",
                drawStart + macroFontSize * ((i * 2) + 1),
                30 * uiScale,
                macroFontSize,
                WHITE
            );
        }
        DrawText(
            "[",
            drawStart - macroFontSize,
            30 * uiScale,
            macroFontSize,
            WHITE
        );
        DrawText(
            "]",
            drawStart + macroFontSize * 2 * curLvl.maxMacroSize,
            30 * uiScale,
            macroFontSize,
            WHITE
        );

        DrawText(
            "Macro",
            (GetScreenWidth() - MeasureText("Macro", macroFontSize)) / 2,
            (int) (10.0f * uiScale),
            macroFontSize,
            WHITE
        );

        EndDrawing();
        frameCtr++;
        frameCtr %= 15;
    }

    RemoveLevel(&curLvl);
    UnloadLevelTextures();
}
