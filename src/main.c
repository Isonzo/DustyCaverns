#include "../include//raylib.h"
#include "../include/rogue.h"
#include <stdlib.h>

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Dusty Caverns");
    SetTargetFPS(12);

    Mob* player = InitPlayer();
    Tile** map = InitMap();
    RandomWalk(map, player);
    int turns = 0;

    while(!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_Q)) break;
        // Vision and input
        ClearFov(player, map);
        bool moved = HandleInput(player, map);
        MakeFov(player, map);

        if(moved)
        {
            // Game logic
        }

        BeginDrawing();
        ClearBackground(BLACK);
        DrawMap(map);
        DrawMob(player);
        EndDrawing();
    }
    CleanMap(map);
    free(player);

    CloseWindow();
    return 0;
}