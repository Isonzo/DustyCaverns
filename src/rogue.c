#include "../include/raylib.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "../include/rogue.h"


struct position
{
    int x, y;
};

struct mob
{
    Position position;
    Color color;
    int radius;
};

struct tile
{
    bool visible;
    bool seen;
    bool walkable;
    bool transparent;
    Color color;
};

// Checks if given y and x coordinate are in bounds.
static bool IsInBounds(int x, int y)
{
    return ((y > 0 && y < SCREEN_HEIGHT/CELL_SIZE - 1) && (x > 0 && x < SCREEN_WIDTH/CELL_SIZE - 1));
}

static bool IsInMap(int x, int y)
{
    return ((y >= 0 && y < SCREEN_HEIGHT/CELL_SIZE) && (x >= 0 && x < SCREEN_WIDTH/CELL_SIZE));
}

// Initializes player, position intending to be set
// When generating the map
Mob* InitPlayer(void)
{
    Mob* player = malloc(sizeof(Mob));
    player->position.x = 20;
    player->position.y = 20;
    player->radius = 15;
    player->color = MAROON;

    return player;
}


// Given the #defined SCREEN_HEIGHT, SCREEN_WIDTH, and CELL_SIZE
// it generates a map that's completely made up of walls.
// You need to apply some generation algorithm to carve it out.
Tile** InitMap(void)
{
    int height, width;
    height = SCREEN_HEIGHT / CELL_SIZE;
    width = SCREEN_WIDTH / CELL_SIZE;
    Tile** map = malloc(SCREEN_WIDTH * sizeof(Tile*));
    for (int i = 0; i < SCREEN_WIDTH; ++i)
    {
        map[i] = malloc(sizeof(Tile) * SCREEN_WIDTH);
        for (int j = 0; j < SCREEN_WIDTH; ++j)
        {
            map[i][j].seen = false;
            map[i][j].visible = false;
            map[i][j].walkable = false;
            map[i][j].color = GRAY;
            map[i][j].transparent = false;
        }
    }

    srand(time(NULL));

    return map;
}

void CleanMap(Tile** map)
{
    int height, width;
    height = SCREEN_HEIGHT / CELL_SIZE;
    width = SCREEN_WIDTH / CELL_SIZE;
    for (int x = 0; x < width; ++x)
    {
        free(map[x]);
    }
    free(map);
}


// Random walk algorithm that automatically places the player and generates a cave-like system.
// Tends to generate spacious organic-looking caverns
void RandomWalk(Tile** map, Mob* player)
{
    int x = 1 + rand() % SCREEN_WIDTH/CELL_SIZE - 1;
    int y = 1 + rand() % SCREEN_HEIGHT/CELL_SIZE - 1;
    int lastX, lastY;
    int count = 0;

    player->position.x = x;
    player->position.y = y;

    do
    {
        if (rand() % 2 == 0)
        {
            if (rand() % 2 == 0)
            {
                if (IsInBounds(x + 1, y))
                {
                    ++x;
                }
                else continue;
            }
            else 
            {
                if (IsInBounds(x - 1, y))
                {
                    --x;
                }
                else continue;
            }
        }
        else 
        {
            if (rand() % 2 == 0)
            {
                if (IsInBounds(x, y + 1))
                {
                    ++y;
                }
                else continue;
            }
            else 
            {
                if (IsInBounds(x, y - 1))
                {
                    --y;
                }
                else continue;
            }

        }
        map[x][y].walkable = true;
        map[x][y].transparent = true;
        map[x][y].color = BROWN;
        ++count;
    } while (count < STEP_AMOUNT);
}

void DrawMap(Tile** map)
{
    for (int x = 0; x < SCREEN_WIDTH/CELL_SIZE; ++x)
    {
        for (int y = 0; y < SCREEN_HEIGHT/CELL_SIZE; ++y)
        {
            if (map[x][y].visible)
            {
                DrawRectangle(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, map[x][y].color);
            }
            else if (map[x][y].seen)
            {
                DrawRectangle(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, Fade(map[x][y].color, 0.6f));
            }
            else
            {
                DrawRectangle(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, BLACK);
            }
        }
    }
}

// Draws Mob at its current position
void DrawMob(Mob* mob)
{
    DrawRectangle(mob->position.x * CELL_SIZE, mob->position.y * CELL_SIZE, CELL_SIZE, CELL_SIZE, mob->color);
}

// Handles input for the player and returns true if player has moved
bool HandleInput(Mob* player, Tile** map)
{
    int dx, dy;
    dx = 0;
    dy = 0;
    if (IsKeyPressed(KEY_J))
    {
        ++dy;
    }
    else if (IsKeyPressed(KEY_K))
    {
        --dy;
    }
    else if (IsKeyPressed(KEY_L))
    {
        ++dx;
    }
    else if (IsKeyPressed(KEY_H))
    {
        --dx;
    }
    else
    {
        return false;
    }
    int newX = player->position.x + dx;
    int newY = player->position.y + dy;

    if (IsInBounds(newX, newY) && map[newX][newY].walkable)
        
    {
        player->position.x = newX;
        player->position.y = newY;
        return true;
    }
    return false;
}

static bool LineOfSight(Position origin, Position target, Tile** map)
{
    if (origin.x == target.x && origin.y == target.y) return true;
    int t, x, y, absDeltaX, absDeltaY, signX, signY, deltaX, deltaY;

    // Get difference of x and y between origin and target positions
    deltaX = origin.x - target.x;
    deltaY = origin.y - target.y;

    absDeltaX = abs(deltaX);
    absDeltaY = abs(deltaY);

    // Get the signs of the delta and store them in a variable
    signX = (deltaX < 0) ? -1: 1;
    signY = (deltaY < 0) ? -1: 1;

    x = target.x;
    y = target.y;

    // Check if the line is x-dominant or y-dominant
    if (absDeltaX > absDeltaY)
    {
        // Run the following if x-dominant
        // t is twice the absolute of y minus the absolute of x
        t = absDeltaY * 2 - absDeltaX;
        do 
        {
            if (t >= 0)
            {
                // If t is greater than or equal to 0
                // add the sign of delta y to y
                // subtract twice the absolute of delta x to from t
                y += signY;
                t -= absDeltaX * 2;
            }
            // add the sign of delta x to x
            // add twice the absolute of delta y to t
            x += signX;
            t += absDeltaY * 2;

            // Check to see if we are at the player's position
            if (x == origin.x && y == origin.y) return true;

            // Keep looping until sight is blocked by non-transparent tile
        }while (map[x][y].transparent);

        // If we exit the loop, our sight must have been blocked
        return false;
    }
    else
    {
        // run the following if y-dominant
        t = absDeltaX * 2 - absDeltaY;
        do
        {
            if (t >= 0)
            {
                x += signX;
                t -= absDeltaY * 2;
            }
            y += signY;
            t += absDeltaX * 2;
            if (x == origin.x && y == origin.y) return true;
        }while (map[x][y].transparent);
        return false;
    }
}

// Return distance between two positions
static int GetDistance(Position origin, Position target)
{ 
  double dy, dx;
  int distance;
  dx = target.x - origin.x;
  dy = target.y - origin.y;
  distance = floor(sqrt((dx * dx) + (dy * dy)));

  return distance;
}

// Make the field of view for the player
void MakeFov(Mob* player, Tile** map)
{
    int x, y, distance;
    int RADIUS = player->radius;

    Position target;

    map[player->position.x][player->position.y].visible = true;
    map[player->position.x][player->position.y].seen = true;

    for (y = player->position.y - RADIUS; y < player->position.y + RADIUS; ++y)
    {
        for (x = player->position.x - RADIUS; x < player->position.x + RADIUS; ++x)
        {
            target.x = x;
            target.y = y;
            distance = GetDistance(player->position, target);

            if (distance < RADIUS)
            {
                if (IsInMap(x, y) && LineOfSight(player->position, target, map))
                {
                    map[x][y].visible = true;
                    map[x][y].seen = true;
                }
            }
        }

    }
}

// Resets visibility for next MakeFov
void ClearFov(Mob* player, Tile** map)
{
    int x, y;
    int RADIUS = player->radius + 1;

    for (y = player->position.y - RADIUS; y < player->position.y + RADIUS; ++y)
    {
        for (x = player->position.x - RADIUS; x < player->position.x + RADIUS; ++x)
        {
            if (IsInMap(x, y))
            {
                map[x][y].visible = false;
            }
        }
    }

}
