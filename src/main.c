#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "collision.h"
#include "poly.h"
#include "textures.h"
#include "colors.h"
#include "ui.h"

#define TILE_AMOUNT 5

enum gameStates
{
	SELECT_TILE,
	TILE_MENU,
	MOVE_TILE,
};

static Tile **board;

VectorTile Board_Size = {25,25};

void DrawHexGrid(VectorTile gridPos, bool filled, int lineThickness, Color color)
{
	Vector2 realPos;
	int size = HEX_RADIUS;

	realPos.x = gridPos.x * (size * 1.66);
	realPos.y = gridPos.y * (size * 1.434);

	if (fmod(abs(gridPos.y), 2) == 1)
		realPos.x += size * HEX_OFFSET;

	if (filled)
		DrawPoly(realPos, 6, size, 0, color);
	else
		DrawPolyLinesEx(realPos, 6, size, 0, lineThickness, color);
}

void DrawTextureGrid(VectorTile gridPos, Texture texture)
{
	Vector2 realPos;
	int size = HEX_RADIUS;
	double scale = 0.2;

	realPos.x = gridPos.x * (size * 1.66);
	realPos.y = gridPos.y * (size * 1.434);

	if (fmod(abs(gridPos.y), 2) == 1)
		realPos.x += size * HEX_OFFSET;

	realPos.x -= ((texture.width	* scale)/2);
	realPos.y -= ((texture.height * scale)/2);

	DrawTextureEx(texture, realPos, 0, scale, WHITE);
}

void DrawBoard(VectorTile selectedHex, enum gameStates gameState)
{
	for (int x = 0; x < Board_Size.y; x++)
		for (int y = 0; y < Board_Size.x; y++)
		{
			switch (board[x][y].player)
			{
			case Player_1:
				DrawHexGrid((VectorTile) {x,y}, true, 10, player1Color);
				break;				 
			case Player_2:
				DrawHexGrid((VectorTile) {x,y}, true, 10, player2Color);
				break;
			case Player_None:
				break;
			}
				
			switch (board[x][y].type)
			{
			case Queen_Bee:
				DrawTextureGrid((VectorTile) {x,y}, beeTexture);
				break;
			case Beetle:
				DrawTextureGrid((VectorTile) {x,y}, beetleTexture);
				break;
			case Grasshopper:
				DrawTextureGrid((VectorTile) {x,y}, grasshopperTexture);
				break;
			case Spider:
				DrawTextureGrid((VectorTile) {x,y}, spiderTexture);
				break;
			case Soldier_Ant:
				DrawTextureGrid((VectorTile) {x,y}, soldierAntTexture);
				break;
			case None:
				break;
			}

			DrawHexGrid((VectorTile) {x,y}, false, 10, GRAY);
		}
	
	if (selectedHex.x <= Board_Size.x &&
			selectedHex.y <= Board_Size.y)
		switch (gameState)
		{
		case SELECT_TILE:
			DrawHexGrid(selectedHex, false, 11, selectTileColor);
			break;
		case TILE_MENU:
			DrawHexGrid(selectedHex, false, 11, menuTileColor);
			break;
		case MOVE_TILE:
			DrawHexGrid(selectedHex, false, 11, moveTileColor);
			break;
		}
}

void EdgeDetection(Camera2D *camera)
{
	bool found0x = false,
	     found0y = false,
	     foundxx = false,
			 foundyy = false;

	for (int x = 0; x < Board_Size.x; x++)
		if (board[x][0].type != None)
			found0y = true;
		
	for (int y = 0; y < Board_Size.y; y++)
		if (board[0][y].type != None)
			found0x = true;
	
	for (int x = 0; x < Board_Size.x; x++)
		if (board[x][Board_Size.y - 1].type != None)
			foundxx = true;
		
	for (int y = 0; y < Board_Size.y; y++)
		if (board[Board_Size.x - 1][y].type != None)
			foundyy = true;

	if (found0x)
	{
		for (int x = Board_Size.x-1; x >= 0; x--)
			for (int y = Board_Size.y-1; y >= 0; y--)
			{
				if (x-1 < 0)
					board[x][y] = (Tile) {None, Player_None};
				else if (board[x-1][y].type != None)
				{
					board[x][y] = board[x-1][y];
					board[x-1][y] = (Tile) {None, Player_None};
				}
			}
		camera->offset.x -= 166 * camera->zoom; // Ugly I know
	}

	if (found0y)
	{
		for (int x = Board_Size.x-1; x >= 0; x--)
			for (int y = Board_Size.y-1; y >= 0; y--)
			{
				if (y-2 < 0)
					board[x][y] = (Tile) {None, Player_None};
				else if (board[x][y-2].type != None)
				{
					board[x][y] = board[x][y-2];
					board[x][y-2] = (Tile) {None, Player_None};
				}
			}
		camera->offset.y -= 287 * camera->zoom;
	}

	if (foundyy)
	{
		Board_Size.y++;
		
		int y = Board_Size.y-1;
		{
			board[y] = MemAlloc((int) Board_Size.y * sizeof(Tile));
			for (int x = 0; x < Board_Size.x; x++)
			{
				board[x][y].type = None;
				board[x][y].player = Player_None;
			}
		}
	}
	
	if (foundxx)
	{
		Board_Size.x++;

		board = MemRealloc(board, Board_Size.x * sizeof(Tile *));
		
		int x = Board_Size.x-1;
		{
			board[x] = MemAlloc((int) Board_Size.y * sizeof(Tile));
			for (int y = 0; y < Board_Size.y; y++)
			{
				board[x][y].type = None;
				board[x][y].player = Player_None;
			}
		}
	}
}

/*
	board = MemAlloc((int) Board_Size.x * sizeof(Tile *));

	for (int x = 0; x < Board_Size.x; x++)
	{
		board[x] = MemAlloc((int) Board_Size.y * sizeof(Tile));
		for (int y = 0; y < Board_Size.y; y++)
		{
			board[x][y].type = None;
			board[x][y].player = Player_None;
		}
	}
*/

int main(int argc, char **argv)
{
	// Vars
	char winName[100] = "Hive";
	char texturepack_path[1024] = "textures/texturepack.zip";

	enum gameStates gameState = SELECT_TILE;

	int oldKey, newKey = KEY_NULL;
	int turn = 0;
	int menuHovered = 0;
	
	Camera2D camera = {{0,0}, {0,0}, 0, 1.0f};

	VectorTile moveTile;
	VectorTile selectedHex = {-1, -1};

	Vector2 winDimensions = {800,800};
	Vector2 menuButtonPos[TILE_AMOUNT];

	board = MemAlloc(Board_Size.x * sizeof(Tile *));

	for (int x = 0; x < Board_Size.x; x++)
	{
		board[x] = MemAlloc(Board_Size.y * sizeof(Tile));
		for (int y = 0; y < Board_Size.y; y++)
		{
			board[x][y].type = None;
			board[x][y].player = Player_None;
		}
	}

	// Parse argv	
	if (argc > 1)
		strcpy(texturepack_path, argv[1]);

	// Init Window
	SetTraceLogLevel(LOG_ERROR);

	SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);

	InitWindow(winDimensions.x, winDimensions.y, winName);

	SetTargetFPS(60);
	SetExitKey(KEY_NULL);

	// Load all textures
	LoadTexturepack(texturepack_path);
	
	SetWindowIcon(windowIcon);

	// Main loop
	while (!WindowShouldClose())
	{
		// Window Dimensions
		if (IsWindowResized())
		{
			winDimensions.x = GetScreenWidth();
			winDimensions.y = GetScreenHeight();
		}

		// Draw
		BeginDrawing();
		{
			ClearBackground(WHITE);

			BeginMode2D(camera);
				DrawBoard(selectedHex, gameState);
			EndMode2D();

			// Draw UI
			DrawTurnBox(turn);
			
			// Draw tile select menu
			if (gameState == TILE_MENU)
				DrawTileSelectMenu(winDimensions, menuButtonPos, TILE_AMOUNT, turn, menuHovered);
		}
		EndDrawing();

		// Key input
		oldKey = newKey;
		newKey = GetKeyPressed();

		if (newKey != oldKey)
		{
			switch (newKey)
			{
			case KEY_EQUAL: // Zoom in
				if (camera.zoom < 1.5)
					camera.zoom += 0.1;
				break;

			case KEY_MINUS: // Zoom out
				if (camera.zoom > 0.5)
					camera.zoom -= 0.1;
				break;

			case KEY_ESCAPE: // Exit menu
				gameState = SELECT_TILE;
				break;

			case KEY_R:	// Reset board (DEBUG)
				for (int x = 0; x < Board_Size.x; x++)
					for (int y = 0; y < Board_Size.y; y++)
						board[x][y] = (Tile) {None, Player_None};
				break;

			case KEY_F11: // Fullscreen window
				{
					int monitor = GetCurrentMonitor();	

					ToggleFullscreen();

					if (IsWindowFullscreen())
						SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
				}
				break;
			}
		}

		// Movement keys
		if (IsKeyDown(KEY_UP))
			camera.offset.y += 10;
		if (IsKeyDown(KEY_DOWN))
			camera.offset.y -= 10;
		if (IsKeyDown(KEY_LEFT))
			camera.offset.x += 10;
		if (IsKeyDown(KEY_RIGHT))
			camera.offset.x -= 10;

		// Mouse input
		Vector2 mousePos = GetMousePosition();
		Vector2 relativeMousePos = GetScreenToWorld2D(mousePos, camera);

		if (gameState != TILE_MENU)
			selectedHex = GetCollisionHexGrid(relativeMousePos);
		else
		{
			bool found = false;

			for (int i = 0; i < TILE_AMOUNT; i++)	
				if(CheckCollisionPointPoly(mousePos, menuButtonPos[i], 6, HEX_RADIUS))
				{
					menuHovered = i;
					found = true;
				}

			if (!found)
				menuHovered = -1;
		}

		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
		{
			switch (gameState)
			{
			case SELECT_TILE:
				if (selectedHex.x < Board_Size.x &&
				    selectedHex.x >= 0)
				{
					if (board[(int) selectedHex.x][(int) selectedHex.y].type == None)
						gameState = TILE_MENU;
					else if ((int) board[(int) selectedHex.x][(int) selectedHex.y].player == turn % 2 + 1)
					{
						gameState = MOVE_TILE;
						moveTile = selectedHex;
					}
				}
				break;

			case TILE_MENU:
				if (menuHovered != -1)
				{
					if (menuHovered == 0)
						board[selectedHex.x][selectedHex.y].type = Soldier_Ant; 
					else 
						board[selectedHex.x][selectedHex.y].type = (menuHovered); 

					board[selectedHex.x][selectedHex.y].player = (turn % 2 + 1);

					turn++;
					gameState = SELECT_TILE;
				}
				break;

			case MOVE_TILE:
				if (selectedHex.x != moveTile.x &&
						selectedHex.x != moveTile.x)
				{
					// Copy tiles 
					board[selectedHex.x][selectedHex.y] = board[moveTile.x][moveTile.y];
					// Delete data in old tiles
					board[moveTile.x][moveTile.y] = (Tile) {None, Player_None};

					turn++;
				}
				gameState = SELECT_TILE;
				break;
			}
		}

		// Move tiles if hit edge
		EdgeDetection(&camera);
	}

exit:
	for (int x = 0; x < Board_Size.x; x++)
		MemFree(board[x]);
	MemFree(board);

	FreeTexturepack();

	CloseWindow();
}
