#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/rayui.h"
#include "../include/level.h"

int main(int argc, char **argv)
{
    InitWindow(500, 500, "The Perfect Run");

    ChangeDirectory(GetDirectoryPath(*argv));
    printf("%s\n", GetDirectoryPath(*argv));
    LoadLevelTextures();

    Level lvl1 = MakeLevel("./levels/lvl2.bin");

    Level curLvl = lvl1;

    Camera2D worldCam = (Camera2D) {
        .target = (Vector2) {
            8.0f * lvl1.width,
            8.0f * lvl1.height
        },
        .zoom = 3.0f,
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
        (Color) {0x1e, 0x1e, 0x1e, 0xe0},
        (Vector2) {0.0f, 0.0f},
        (Vector2) {(float) GetScreenWidth(), 50.0f},
        &uiScale
    );

    Element bottomBar = MakeBox(
        (Color) {0x1e, 0x1e, 0x1e, 0xe0},
        (Vector2) {0.0f, (((float)GetScreenHeight())/ uiScale) - 50.0f},
        (Vector2) {(float) GetScreenWidth(), 50.0f},
        &uiScale
    );

    Element leftTopBtnBox = MakeTextBox(
        (Color) {0x1e, 0x1e, 0x1e, 0x00},
        (Vector2) {0.0f, 0.0f},
        (Vector2) {50.0f, 50.0f},
        &uiScale,
        GRAY,
        (Vector2) {20.0f, 0.0f},
        "<",
        50,
        &uiScale
    );

    Element rightTopBtnBox = MakeTextBox(
        (Color) {0x1e, 0x1e, 0x1e, 0x00},
        (Vector2) {((float) GetScreenWidth()) - 50.0f, 0.0f},
        (Vector2) {50.0f, 50.0f},
        &uiScale,
        GRAY,
        (Vector2) {20.0f, 0.0f},
        ">",
        50,
        &uiScale
    );

    Element editBtnBox = MakeTextBox(
        (Color) {0x1e, 0x1e, 0x1e, 0x00},
        (Vector2) {0.0f, bottomBar.pos.y},
        (Vector2) {130.0f, 100.0f},
        &uiScale,
        WHITE,
        (Vector2) {20.0f, 20.0f},
        "Run loop",
        20,
        &uiScale
    );

    Element allowInputBtnBox = MakeTextBox(
        (Color) {0x1e, 0x1e, 0x1e, 0x00},
        (Vector2) {130.0f, bottomBar.pos.y},
        (Vector2) {130.0f, 100.0f},
        &uiScale,
        WHITE,
        (Vector2) {20.0f, 20.0f},
        "Edit Loop",
        20,
        &uiScale
    );

    Element ffBtnBox = MakeTextBox(
        (Color) {0x1e, 0x1e, 0x1e, 0x00},
        (Vector2) {260.0f, bottomBar.pos.y},
        (Vector2) {165.0f, 100.0f},
        &uiScale,
        WHITE,
        (Vector2) {20.0f, 20.0f},
        "Fast foward",
        20,
        &uiScale
    );

    Button ffButton = MakeElemButton(
        NULL, &ffBtnBox, BUTTON_INPUT_DOWN, BUTTON_OUTPUT_SELECT
    );

    Button allowInputButton = MakeElemButton(
        NULL, &allowInputBtnBox, BUTTON_INPUT_ON, BUTTON_OUTPUT_SELECT
    );

    Button editButton = MakeElemButton(
        NULL, &editBtnBox, BUTTON_INPUT_ON, BUTTON_OUTPUT_TOGGLE
    );
    editButton.output = 1;

    Button rtButton = MakeElemButton(
        NULL, &rightTopBtnBox, BUTTON_INPUT_ON, BUTTON_OUTPUT_FRAME
    );

    Button ltButton = MakeElemButton(
        NULL, &leftTopBtnBox, BUTTON_INPUT_STAY, BUTTON_OUTPUT_FRAME
    );

    TextInput macroInput = MakeTextInput(
        curLvl.player.macro,
        curLvl.maxMacroSize,
        &allowInputButton.output,
        12,
        (struct CharRange[]) {
            {'w', 'w'}, {'a', 'a'}, {'s', 's'}, {'d', 'd'}, {'l', 'l'},
            {'W', 'W'}, {'A', 'A'}, {'S', 'S'}, {'D', 'D'}, {'r', 'r'} ,
            {'u', 'u'}, {'-', '-'}
        }
    );

    float speed;

    SetTargetFPS(30);

    int frameCtr = 0;

    char playerDead = 0;
    char playerTurn = 0;

    strcpy(curLvl.player.macro, "");

    while (!WindowShouldClose())
    {
        ClearBackground(BLACK);

        uiScale = ((float) GetScreenWidth()) / 500.0f;

        Zoom(&(worldCam.zoom), 1.0f, 64.0f, 0.125f);
        CenterCamera(&worldCam);
        worldCamDrag.clicked = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
        UpdateDrag(&worldCamDrag);

        bottomBar.pos.y = (((float)GetScreenHeight())/ uiScale) - 50.0f;
        editBtnBox.pos.y = bottomBar.pos.y;
        allowInputBtnBox.pos.y = bottomBar.pos.y;
        ffBtnBox.pos.y = bottomBar.pos.y;

        char prevEdit = editButton.output;
        UpdateButton(&editButton);
        if(editButton.output)UpdateButton(&allowInputButton);
        else allowInputButton.output = 0;

        if(strlen(curLvl.player.macro) != 0) {
            editBtnBox.data.textbox->color.a =
                editButton.mouseOn ? 0xff : 0x00;
            editBtnBox.data.textbox->textColor =
                editButton.mouseOn ? RED : WHITE;
        } else if (editButton.mouseOn){
            char blink = (frameCtr < 10);
            allowInputBtnBox.data.textbox->color.a = blink ? 0xff : 0x00;
            allowInputBtnBox.data.textbox->textColor = blink ? RED : WHITE;
        }

        if(editButton.output && !editButton.mouseOn)
        {
            allowInputBtnBox.data.textbox->color.a =
                allowInputButton.mouseOn ? 0xff : 0x00;
            allowInputBtnBox.data.textbox->textColor =
                allowInputButton.mouseOn ? RED : WHITE;
        } else {
            if(strlen(curLvl.player.macro) == 0) {
                editButton.output = 1;
            } else {
                allowInputBtnBox.data.textbox->color.a = 0x00;
                allowInputBtnBox.data.textbox->textColor = WHITE;
            }
            if (!editButton.output){
                allowInputBtnBox.data.textbox->textColor = GRAY;
                allowInputBtnBox.data.textbox->color.a = 0x00;
            }
        }

        if(
            allowInputButton.output && (
                (allowInputBtnBox.data.textbox->textColor.r == 255) &&
                (allowInputBtnBox.data.textbox->textColor.g == 255) &&
                (allowInputBtnBox.data.textbox->textColor.b == 255)
            )
        )
            allowInputBtnBox.data.textbox->textColor = PINK;

        editBtnBox.data.textbox->text = editButton.output ?
            "Run Loop" : "End Loop";

        if(!editButton.output)
        {
            macroInput.focused = 0;
            UpdateButton(&ffButton);
            ffBtnBox.data.textbox->color.a =
                ffButton.mouseOn ? 0xff : 0x00;
            ffBtnBox.data.textbox->textColor =
                ffButton.mouseOn ? RED : WHITE;
        } else ffBtnBox.data.textbox->textColor = GRAY;

        speed = ffButton.output ? (
            (curLvl.enemyNum == 0) ? 0.25 : 0.5
        ) : 0.075;

        DrawLevel(curLvl, worldCam, frameCtr);

        if(editButton.output)
        {
            if(!prevEdit)
            {
                playerTurn = 0;
                ResetMacroBot(&(curLvl.player));
                for(int i = 0; i < curLvl.enemyNum; i++)
                    ResetMacroBot(curLvl.enemies + i);
            }

            rightTopBtnBox.data.textbox->textColor = GRAY;
            rightTopBtnBox.data.textbox->color.a = 0x00;
            UpdateTextInput(&macroInput);
        } else if(
            (curLvl.player.posX == curLvl.player.goalX) &&
            (curLvl.player.posY == curLvl.player.goalY)
        )
        {
            UpdateButton(&rtButton);
            rightTopBtnBox.data.textbox->textColor =
                ((frameCtr < 10) || rtButton.mouseOn) ? RED : WHITE;
            rightTopBtnBox.data.textbox->color.a = 
                ((frameCtr < 10) || rtButton.mouseOn) ? 0xff : 0x00;

            if(rtButton.output)
            {
                if(curLvl.enemyNum < curLvl.maxEnemies)
                {
                    ResetMacroBot(&(curLvl.player));
                    curLvl.enemyNum++;
                    curLvl.player = curLvl.enemies[curLvl.enemyNum];
                    if(curLvl.enemies[curLvl.enemyNum].macro == NULL)
                        curLvl.enemies[curLvl.enemyNum].macro = (char*)
                            malloc(sizeof(int) * curLvl.maxMacroSize);
                    curLvl.player = curLvl.enemies[curLvl.enemyNum];
                    macroInput.output = curLvl.player.macro;
                    *(macroInput.output) = 0;
                    macroInput.curLen = 0;
                }
            }

        } else if(curLvl.enemyNum == 0)
        {
            playerDead = curLvl.player.curAnimation ==
                MACRO_BOT_ANIMATION_DEAD;
            RunMacroBot(&(curLvl.player), speed);
            if(curLvl.player.curAnimation == MACRO_BOT_ANIMATION_NONE)
            {
                if(playerDead)
                {
                    editButton.output = 1;
                    ResetMacroBot(&(curLvl.player));
                } else UpdatePlayerBotAnimation(&curLvl);
            }
        } else {
            if(playerTurn)
            {
                RunMacroBot(&(curLvl.player), speed);

                if(curLvl.player.curAnimation == MACRO_BOT_ANIMATION_GONE)
                    curLvl.player.curAnimation = MACRO_BOT_ANIMATION_NONE;

                if(curLvl.player.curAnimation == MACRO_BOT_ANIMATION_NONE)
                {
                    playerTurn = 0;

                    if(playerDead)
                    {
                        editButton.output = 1;
                        ResetMacroBot(&(curLvl.player));
                        playerTurn = 0;
                        for(int i = 0; i < curLvl.enemyNum; i++)
                            ResetMacroBot(curLvl.enemies + i);
                    } else UpdateEnemyBotAnimation(&curLvl);
                }
            } else {
                playerTurn = 1;
                for(int i = 0; i < curLvl.enemyNum; i++)
                {
                    RunMacroBot(curLvl.enemies + i, speed);
                    playerTurn &= (
                        curLvl.enemies[i].curAnimation == 
                        MACRO_BOT_ANIMATION_NONE) || (
                        curLvl.enemies[i].curAnimation == 
                        MACRO_BOT_ANIMATION_GONE
                    );
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
            GetScreenWidth() - (macroFontSize * curLvl.maxMacroSize * 2) +
            macroFontSize
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

            if(
                (((i + 1) % macroLen) == curLvl.player.macroPos) &&
                ((!editButton.output) || macroInput.focused)
            )
            {
                DrawText(
                    "^",
                    drawStart + macroFontSize * i * 2,
                    40 * uiScale,
                    macroFontSize,
                    (
                        macroInput.focused && IsMouseButtonDown(MOUSE_BUTTON_LEFT)
                    ) ?
                        RED : WHITE
                );
            }
        }

        DrawText(
            "[",
            drawStart - macroFontSize,
            30 * uiScale,
            macroFontSize,
            WHITE
        );

        if((macroLen == 0) && macroInput.focused)
            DrawText(
                "No need to click on this, just start typing",
                drawStart,
                30 * uiScale,
                macroFontSize,
                (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) ? RED : GRAY
            );
        else {
            for(int i = 0; i < curLvl.maxMacroSize; i++)
            {
                if(!(i + 1 < curLvl.maxMacroSize))break;

                DrawText(
                    ",",
                    drawStart + macroFontSize * ((i * 2) + 1),
                    30 * uiScale,
                    macroFontSize,
                    GRAY
                );
            }
        }

        DrawText(
            "]",
            drawStart + macroFontSize * 2 * curLvl.maxMacroSize - (
                macroFontSize / 2
            ),
            30 * uiScale,
            macroFontSize,
            WHITE
        );

        char topStr[21];

        sprintf(topStr, "Iteration %d", curLvl.enemyNum + 1);

        DrawText(
            topStr,
            (GetScreenWidth() - MeasureText(topStr, macroFontSize * 2)) / 2,
            (int) (5.0f * uiScale),
            macroFontSize * 2,
            WHITE
        );

        DrawElement(leftTopBtnBox);
        DrawElement(rightTopBtnBox);
        DrawElement(bottomBar);
        DrawElement(allowInputBtnBox);
        DrawElement(editBtnBox);
        DrawElement(ffBtnBox);

        EndDrawing();
        frameCtr++;
        frameCtr %= 15;
    }

    FreeElement(&leftTopBtnBox);
    FreeElement(&rightTopBtnBox);
    FreeElement(&bottomBar);
    FreeElement(&editBtnBox);
    FreeElement(&allowInputBtnBox);
    FreeElement(&topBar);

    RemoveLevel(&curLvl);
    UnloadLevelTextures();
    CloseWindow();
}
