#define HEX_RADIUS 100
#define HEX_OFFSET 0.83

enum TileTypes
{
  None,
  Queen_Bee,
  Beetle,
  Grasshopper,
  Spider,
  Soldier_Ant,
};

enum Players
{
  Player_None,
  Player_1,
  Player_2,
};

typedef struct Tile
{
  enum TileTypes type;
  enum Players player;
} Tile;

typedef struct VectorTile
{
	int x, y;
} VectorTile;
