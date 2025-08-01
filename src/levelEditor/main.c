#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{

    char *arr = malloc(1);
    *arr = 0;

    int botNum = 0;
    int curBot = -1;
    int *goalX = malloc(0);
    int *goalY = malloc(0);
    int *spawnX = malloc(0);
    int *spawnY = malloc(0);
    int *spawnR = malloc(0);

    int width = 1, height = 1, tile = 0;

    InitWindow(500, 500, "level editor");

    SetTargetFPS(30);

    while (!WindowShouldClose())
    {
        ClearBackground(WHITE);

        if(IsKeyPressed(KEY_ZERO))tile = 0;
        if(IsKeyPressed(KEY_ONE))tile = 1;
        if(IsKeyPressed(KEY_THREE))tile = 3;

        if(
            IsKeyPressed(KEY_EQUAL) || IsKeyPressed(KEY_MINUS) ||
            IsKeyPressedRepeat(KEY_EQUAL) || IsKeyPressedRepeat(KEY_MINUS)

        )
        {
            int oldHeight = height, oldWidth = width;
            if(IsKeyPressed(KEY_EQUAL) || IsKeyPressedRepeat(KEY_EQUAL))
            {
                if(IsKeyDown(KEY_W))width++;
                if(IsKeyDown(KEY_H))height++;
            }
            if(IsKeyPressed(KEY_MINUS) || IsKeyPressedRepeat(KEY_MINUS))
            {
                if(IsKeyDown(KEY_W))width--;
                if(IsKeyDown(KEY_H))height--;
            }

            if(width < 1)width = 1;
            if(height < 1)height = 1;

            int minHeight = (height < oldHeight)? height : oldHeight;
            int minWidth = (width < oldWidth)? width : oldWidth;
            char *tempArr = malloc(width * height);

            for(int i = 0; i < (width * height); i++) tempArr[i] = 0;

            for(int i = 0; i < minHeight; i++)
            {
                for(int j = 0; j < minWidth; j++)
                    tempArr[j + i * width] = arr[j + i * oldWidth];
            }

            free(arr);
            arr = tempArr;
        }

        if(
            IsKeyPressed(KEY_EQUAL) || IsKeyPressed(KEY_MINUS)
        )
        {
            int oldBotNum = botNum;
            if(IsKeyPressed(KEY_EQUAL) && IsKeyDown(KEY_B))botNum++;
            if(IsKeyPressed(KEY_MINUS) && IsKeyDown(KEY_B))botNum--;

            if(botNum < 0)botNum = 0;

            int minBot = (botNum < oldBotNum) ? botNum : oldBotNum;

            int *t1 = (int*) malloc(sizeof(int) * botNum);
            int *t2 = (int*) malloc(sizeof(int) * botNum);
            int *t3 = (int*) malloc(sizeof(int) * botNum);
            int *t4 = (int*) malloc(sizeof(int) * botNum);
            int *t5 = (int*) malloc(sizeof(int) * botNum);

            for(int i = 0; i < botNum; i++)
            {
                t1[i] = 0;
                t2[i] = 0;
                t3[i] = 0;
                t4[i] = 0;
                t5[i] = 0;
            }

            for(int i = 0; i < minBot; i++)
            {
                t1[i] = spawnX[i];
                t2[i] = spawnY[i];
                t3[i] = goalX[i];
                t4[i] = goalY[i];
                t5[i] = spawnR[i];
            }

            free(spawnX);
            free(spawnY);
            free(goalX);
            free(goalY);
            free(spawnR);

            spawnX = t1;
            spawnY = t2;
            goalX = t3;
            goalY = t4;
            spawnR = t5;

        }

        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            int x, y;

            x = ((int) GetMouseX()) / 10;
            y = ((int) GetMouseY()) / 10;

            if((x > -1) && (y > -1) && (x < width) && (y < height))
                arr[x + y * width] = tile;
        }

        if(IsKeyPressed(KEY_RIGHT))curBot++;
        if(IsKeyPressed(KEY_LEFT))curBot--;

        if(curBot > -1)
        {

            int x, y;

            x = ((int) GetMouseX()) / 10;
            y = ((int) GetMouseY()) / 10;

            if((x > -1) && (y > -1) && (x < width) && (y < height))
            {
                if(IsKeyDown(KEY_S))
                {
                    spawnX[curBot] = x;
                    spawnY[curBot] = y;
                }
                if(IsKeyDown(KEY_G))
                {
                    goalX[curBot] = x;
                    goalY[curBot] = y;
                }
            }

            if(IsKeyPressed(KEY_R))spawnR[curBot] = (spawnR[curBot] + 1) & 3;
        } else {
            curBot = 0;
        }

        if(IsKeyPressed(KEY_M))
        {
            FILE *fptr = fopen("lvl.bin", "wb+");

            fwrite(&width, 4, 1, fptr);
            fwrite(&height, 4, 1, fptr);
            fwrite(arr, 1, width * height, fptr);
            fwrite(&botNum, 4, 1, fptr);
            fwrite(goalX, 4, botNum, fptr);
            fwrite(goalY, 4, botNum, fptr);
            fwrite(spawnX, 4, botNum, fptr);
            fwrite(spawnY, 4, botNum, fptr);
            fwrite(spawnR, 4, botNum, fptr);

            fclose(fptr);
        }

        if(curBot >= botNum)curBot = botNum - 1;

        for(int i = 0; i < height; i++)
        {
            for(int j = 0; j < width; j++)
            {
                switch(arr[j + i * width])
                {
                    case 0:
                        DrawRectangle(j * 10, i * 10, 10, 10, BLACK);
                        break;
                    case 1:
                        DrawRectangle(j * 10, i * 10, 10, 10, DARKGREEN);
                        break;
                    case 3:
                        DrawRectangle(j * 10, i * 10, 10, 10, MAROON);
                        break;
                }
            }
        }

        for(int i = 0; i < botNum; i++)
        {
            char tmp[] = {0, 0};
            tmp[0] = "^>v<"[spawnR[i]];
            DrawText(tmp, spawnX[i] * 10, spawnY[i] * 10, 10,
                (i == curBot) ? PINK : WHITE
            );
            DrawText("G", goalX[i] * 10, goalY[i] * 10, 10,
                (i == curBot) ? PINK : WHITE 
            );
        }

        EndDrawing();
    }

    free(arr);
}
