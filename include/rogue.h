#ifndef ROGUE_H
#define ROGUE_H

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 450
#define CELL_SIZE 10
#define STEP_AMOUNT 5000

typedef struct position Position;
typedef struct mob Mob;
typedef struct tile Tile;

// Initialization
Mob* InitPlayer(void);
Tile** InitMap(void);
void RandomWalk(Tile** map, Mob* player);

// Draw Functions
void DrawMap(Tile** map);
void DrawMob(Mob* mob);

// Helper Functions
bool HandleInput(Mob* player, Tile** map);
void CleanMap(Tile** map);
int GetVisibility(Tile** map);

// FOV Functions
void MakeFov(Mob* player, Tile** map);
void ClearFov(Mob* player, Tile** map);

#endif