#include "../include/raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/rayui.h"
#include "../include/level.h"

int main(int argc, char **argv)
{
    InitWindow(1000, 750, "MACRO - by River Dewberry");
    InitAudioDevice();

    ChangeDirectory(GetDirectoryPath(*argv));
    printf("%s\n", GetDirectoryPath(*argv));
    LoadLevelTextures();

    Music music = LoadMusicStream("./audio/music.mp3");
    PlayMusicStream(music);

    int numLevels = 8;
    Level levels[] = {
        MakeLevel("./levels/lvl1.bin"),
        MakeLevel("./levels/lvl2.bin"),
        MakeLevel("./levels/lvl3.bin"),
        MakeLevel("./levels/lvl8.bin"),
        MakeLevel("./levels/lvl6.bin"),
        MakeLevel("./levels/lvl4.bin"),
        MakeLevel("./levels/lvl7.bin"),
        MakeLevel("./levels/lvl5.bin")
    };

    char *levelNames[] = {
        "Tutorial",
        "Warming up",
        "Compact",
        "Obstacle course",
        "Backtracking",
        "Insanity",
        "patterns",
        "Open paths"
    };

    ResetLevel(levels);
    Level curLvl = levels[0];

    Camera2D worldCam = (Camera2D) {
        .target = (Vector2) {
            8.0f * curLvl.width,
            8.0f * curLvl.height
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
            (float)(curLvl.width * 16),
            (float)(curLvl.height * 16)
        }},
        DRAGGER_BOUND_BOX,
        &camDragScale,
        &worldCam
    );

    float uiScale = ((float) GetScreenWidth()) / 500.0f;

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
        (Vector2) {15.0f, 0.0f},
        "<",
        50,
        &uiScale
    );

    Element rightTopBtnBox = MakeTextBox(
        (Color) {0x1e, 0x1e, 0x1e, 0x00},
        (Vector2) {((float) GetScreenWidth())/uiScale - 50.0f, 0.0f},
        (Vector2) {50.0f, 50.0f},
        &uiScale,
        GRAY,
        (Vector2) {20.0f, 0.0f},
        ">",
        50,
        &uiScale
    );

    Element infoBtnBox = MakeTextBox(
        (Color) {0x1e, 0x1e, 0x1e, 0x00},
        (Vector2) {((float) GetScreenWidth())/uiScale - 50.0f, 50.0f},
        (Vector2) {50.0f, 50.0f},
        &uiScale,
        GRAY,
        (Vector2) {10.0f, 0.0f},
        "?",
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

    char endText[150];

    Element endBox = MakeTextBox(
        (Color) {0x1e, 0x1e, 0x1e, 0xe0},
        (Vector2) {10.0f, 60.0f},
        (Vector2) {480.0f, 150.0f},
        &uiScale,
        WHITE,
        (Vector2) {20.0f, 20.0f},
        endText,
        20,
        &uiScale
    );

    Element infoBox = MakeTextBox(
        (Color) {0x1e, 0x1e, 0x1e, 0xe0},
        (Vector2) {0.0f, 50.0f},
        (Vector2) {500.0f, 177.0f},
        &uiScale,
        WHITE,
        (Vector2) {10.0f, 0.0f},
        "Click 'edit loop', then start typing instructions\n"
        "After you are finished typing instructions, click 'run loop'\n\n"
        "Robot instruction key:\n"
        "'w' - move forward relative to the direction the robot is facing\n"
        "'a' - move left relative to the direction the robot is facing\n"
        "'s' - move down relative to the direction the robot is facing\n"
        "'d' - move right relative to the direction the robot is facing\n"
        "'W' - move up regardless of the direction the robot is facing\n"
        "'A' - move left regardless of the direction the robot is facing\n"
        "'S' - move down regardless of the direction the robot is facing\n"
        "'D' - move right regardless of the direction the robot is facing\n"
        "'l' - turn the robot 90 degrees to the left\n"
        "'r' - turn the robot 90 degrees to the right\n"
        "'u' - turn the robot 180 degrees\n"
        "'-' - do nothing (useful for timing)\n",
        10,
        &uiScale
    );

    Element exitBtnBox = MakeTextBox(
        (Color) {0x1e, 0x1e, 0x1e, 0x00},
        (Vector2) {425.0f, bottomBar.pos.y},
        (Vector2) {75.0f, 100.0f},
        &uiScale,
        WHITE,
        (Vector2) {20.0f, 20.0f},
        "Exit",
        20,
        &uiScale
    );

    Button exitButton = MakeElemButton(
        NULL, &exitBtnBox, BUTTON_INPUT_DOWN, BUTTON_OUTPUT_FRAME
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

    Button infoButton = MakeElemButton(
        NULL, &infoBtnBox, BUTTON_INPUT_ON, BUTTON_OUTPUT_FRAME
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

    char inLevel = 1;
    char checkedInfo = 0;
    int lvlPlaying = 0;

    float levelSelectScroll = 0.0f;

    while (!WindowShouldClose())
    {
        UpdateMusicStream(music);
        ClearBackground(BLACK);

        uiScale = ((float) GetScreenWidth()) / 500.0f;

        if(!inLevel)
        {
            checkedInfo = 1;
            CenterCamera(&worldCam);
            levelSelectScroll -= GetMouseWheelMove() * 25.0f * uiScale;

            int maxY = (numLevels >> 2) + ((numLevels & 3) != 0) - 1;

            if(
                (levelSelectScroll + (180.0f * uiScale)) > GetScreenHeight())
                levelSelectScroll = GetScreenHeight() - (180.0f * uiScale);


            if(
                (levelSelectScroll + (
                    120.0f * ((float)maxY)
                ) * uiScale) < 0.0f
            )
                levelSelectScroll = -120.0f * ((float)maxY) * uiScale;

            for(int i = 0; i < numLevels; i++)
            {
                int x = i & 3;
                int y = i >> 2;

                char mouseOn = CheckCollisionPointRec(
                    GetMousePosition(),
                    (Rectangle) {
                        (20 + (x * 120)) * uiScale,
                        (60 + (y * 120)) * uiScale + levelSelectScroll,
                        100 * uiScale,
                        100 * uiScale
                    }
                );

                DrawRectangle(
                    (20 + (x * 120)) * uiScale,
                    (60 + (y * 120)) * uiScale + levelSelectScroll,
                    100 * uiScale,
                    100 * uiScale,
                    (Color) {0x1e, 0x1e, 0x1e, mouseOn ? 0xFF : 0xe0}
                );

                char numBuf[8];
                sprintf(numBuf, "%d", i);

                DrawText(
                    numBuf,
                    (30 + (x * 120)) * uiScale,
                    (70 + (y * 120)) * uiScale + levelSelectScroll,
                    20 * uiScale,
                    mouseOn ? RED : WHITE
                );

                DrawText(
                    levelNames[i],
                    (30 + (x * 120)) * uiScale,
                    (140 + (y * 120)) * uiScale + levelSelectScroll,
                    10 * uiScale,
                    mouseOn ? WHITE : GRAY
                );

                if(IsMouseButtonDown(MOUSE_BUTTON_LEFT) && mouseOn)
                {
                    ResetLevel(levels + i);
                    curLvl = levels[i];
                    lvlPlaying = i;
                    macroInput.output = curLvl.player.macro;
                    macroInput.curLen = 0;
                    editButton.output = 1;
                    allowInputButton.output = 0;
                    inLevel = 1;
                    worldCamDrag.clicked = 0;
                    UpdateDrag(&worldCamDrag);
                    worldCamDrag.bound.rec[0] = (Rectangle) {
                        0.0f,
                        0.0f,
                        (float)(curLvl.width * 16),
                        (float)(curLvl.height * 16)
                    };
                    worldCam.target = (Vector2) {
                        8.0f * curLvl.width,
                        8.0f * curLvl.height
                    };
                    worldCam.zoom = 3.0f;
                    checkedInfo = i != 0;
                }
            }

            DrawRectangle(0, 0, GetScreenWidth(), 60 * uiScale, BLACK);

            DrawText(
                "MACRO - level select",
                20 * uiScale,
                10 * uiScale,
                40 * uiScale,
                WHITE
            );

            EndDrawing();

            continue;
        }

        Zoom(&(worldCam.zoom), 1.0f, 64.0f, 0.125f);
        CenterCamera(&worldCam);
        worldCamDrag.clicked = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
        UpdateDrag(&worldCamDrag);

        UpdateButton(&exitButton);
        exitBtnBox.data.textbox->color.a = exitButton.mouseOn ? 0xff : 0x00;
        exitBtnBox.data.textbox->textColor = exitButton.mouseOn ? RED : WHITE;
        if(exitButton.output)inLevel = 0;

        bottomBar.pos.y = (((float)GetScreenHeight())/ uiScale) - 50.0f;
        editBtnBox.pos.y = bottomBar.pos.y;
        allowInputBtnBox.pos.y = bottomBar.pos.y;
        ffBtnBox.pos.y = bottomBar.pos.y;
        exitBtnBox.pos.y = bottomBar.pos.y;
        infoBox.dim.y = bottomBar.pos.y - infoBox.pos.y;

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
            editBtnBox.data.textbox->color.a = 0x00;
            editBtnBox.data.textbox->textColor = WHITE;
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

        UpdateButton(&ffButton);
        if(!editButton.output)
        {
            macroInput.focused = 0;
            ffBtnBox.data.textbox->color.a =
                ffButton.mouseOn ? 0xff : 0x00;
            ffBtnBox.data.textbox->textColor =
                ffButton.mouseOn ? RED : WHITE;
        } else ffBtnBox.data.textbox->textColor = GRAY;

        speed = ffButton.output ? (
            (curLvl.enemyNum == 0) ? 0.25 : 0.5
        ) : 0.075;

        DrawLevel(curLvl, worldCam, frameCtr);

        UpdateButton(&ltButton);
        if(
            (
                editButton.output ||
                ((curLvl.player.posX == curLvl.player.goalX) &&
                (curLvl.player.posY == curLvl.player.goalY))
            ) && ltButton.output && (curLvl.enemyNum > 0))
        {
            ResetMacroBot(&(curLvl.player));
            curLvl.enemyNum--;
            curLvl.player = curLvl.enemies[curLvl.enemyNum];
            macroInput.output = curLvl.player.macro;
            macroInput.curLen = strlen(macroInput.output);
            UpdateEnemyTiles(&curLvl);
            editButton.output = 1;
        } else {
            leftTopBtnBox.data.textbox->color.a = 0x00;
            leftTopBtnBox.data.textbox->textColor = GRAY;
        }

        if(editButton.output)
        {
            if(curLvl.enemyNum > 0)
            {
                leftTopBtnBox.data.textbox->color.a =
                    ltButton.mouseOn ? 0xFF : 0x00;
                leftTopBtnBox.data.textbox->textColor =
                    ltButton.mouseOn ? RED : WHITE;
            }

            if(!prevEdit)
            {
                playerTurn = 0;
                ResetMacroBot(&(curLvl.player));
                for(int i = 0; i < curLvl.enemyNum; i++)
                    ResetMacroBot(curLvl.enemies + i);
            }

            rightTopBtnBox.data.textbox->textColor = GRAY;
            rightTopBtnBox.data.textbox->color.a = 0x00;

            char key = 0;
            if(IsKeyPressed(KEY_W))key = 'w';
            if(IsKeyPressed(KEY_A))key = 'a';
            if(IsKeyPressed(KEY_S))key = 's';
            if(IsKeyPressed(KEY_D))key = 'd';
            if(
                (IsKeyDown(KEY_LEFT_SHIFT) ||
                IsKeyDown(KEY_RIGHT_SHIFT)) &&
                (key != 0)
            )key -= 'a' - 'A';
            if(IsKeyPressed(KEY_L))key = 'l';
            if(IsKeyPressed(KEY_R))key = 'r';
            if(IsKeyPressed(KEY_U))key = 'u';
            if(IsKeyPressed(KEY_MINUS))key = '-';

            if(macroInput.focusPtr[0])
            {
                if((macroInput.curLen < macroInput.maxLen) && (key != 0))
                {
                    macroInput.output[macroInput.curLen] = key;
                    macroInput.curLen++;
                    macroInput.output[macroInput.curLen] = 0;
                }

                if(
                    (IsKeyPressed(KEY_BACKSPACE) ||
                    IsKeyPressedRepeat(KEY_BACKSPACE)) && (
                        macroInput.curLen > 0
                    )
                )
                {
                    macroInput.curLen--;
                    macroInput.output[macroInput.curLen] = 0;
                }
            }

            macroInput.focused = macroInput.focusPtr[0];

        } else if(
            (curLvl.player.posX == curLvl.player.goalX) &&
            (curLvl.player.posY == curLvl.player.goalY)
        )
        {
            if(curLvl.enemyNum > 0)
            {
                leftTopBtnBox.data.textbox->color.a =
                    ltButton.mouseOn ? 0xFF : 0x00;
                leftTopBtnBox.data.textbox->textColor =
                    ltButton.mouseOn ? RED : WHITE;
            }

            UpdateButton(&rtButton);
            rightTopBtnBox.data.textbox->textColor =
                ((frameCtr < 10) || rtButton.mouseOn) ? RED : WHITE;
            rightTopBtnBox.data.textbox->color.a = 
                ((frameCtr < 10) || rtButton.mouseOn) ? 0xff : 0x00;

            if(!(curLvl.enemyNum < curLvl.maxEnemies) && rtButton.mouseOn)
            {
                int instrCtr = 0;

                for(int i = 0; i < (curLvl.maxEnemies + 1); i++)
                    instrCtr += strlen(curLvl.enemies[i].macro);

                sprintf(endText,
                    "You beat level %d using %d instructions!\n"
                    "Click the exit button to end the level\n\n"
                    "For an extra challenge,\ntry to use as few instructions"
                    " as you can\n", lvlPlaying, instrCtr
                );

                endBox.data.textbox->text = endText;

                DrawElement(endBox);
            }

            if(rtButton.output)
            {
                if(curLvl.enemyNum < curLvl.maxEnemies)
                {
                    ResetMacroBot(&(curLvl.player));
                    curLvl.enemyNum++;
                    if(curLvl.enemies[curLvl.enemyNum].macro == NULL)
                    {
                        curLvl.enemies[curLvl.enemyNum].macro = (char*)
                            malloc(sizeof(char) * (curLvl.maxMacroSize + 1));

                        *(curLvl.enemies[curLvl.enemyNum].macro) = 0;
                        macroInput.curLen = 0;
                    } else {
                        macroInput.curLen = strlen(
                            curLvl.enemies[curLvl.enemyNum].macro
                        );
                    }
                    curLvl.player = curLvl.enemies[curLvl.enemyNum];
                    macroInput.output = curLvl.player.macro;
                    macroInput.focusPtr[0] = 1;
                    editButton.output = 1;
                    UpdateEnemyTiles(&curLvl);
                }
            }
        } else if(curLvl.enemyNum == 0)
        {
            if(
                curLvl.player.curAnimation ==
                MACRO_BOT_ANIMATION_GONE
            )
            {
                editButton.output = 1;
                ResetMacroBot(&(curLvl.player));
            } else {
                RunMacroBot(&(curLvl.player), speed);

                if(curLvl.player.curAnimation == MACRO_BOT_ANIMATION_NONE)
                    UpdatePlayerBotAnimation(&curLvl);
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

        sprintf(
            topStr,
            "Iteration %d/%d",
            curLvl.enemyNum + 1,
            curLvl.maxEnemies + 1
        );

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
        DrawElement(exitBtnBox);

        UpdateButton(&infoButton);

        if(infoButton.mouseOn)
        {
            checkedInfo = 1;
            DrawElement(infoBox);
        }

        if(checkedInfo)
            infoBtnBox.data.textbox->textColor = GRAY;
        else
            infoBtnBox.data.textbox->textColor = (frameCtr < 10)? RED : WHITE;

        DrawElement(infoBtnBox);

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
    FreeElement(&exitBtnBox);
    FreeElement(&ffBtnBox);
    FreeElement(&endBox);
    FreeElement(&infoBox);
    FreeElement(&infoBtnBox);
    StopMusicStream(music);
    for(int i = 0; i < numLevels; i++)
        RemoveLevel(levels + i);
    UnloadLevelTextures();
    CloseWindow();
    CloseAudioDevice();
}
