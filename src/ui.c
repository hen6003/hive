#include "tiles.h"
#include "colors.h"
#include "textures.h"

void DrawTileSelectMenu(Vector2 winDimensions, Vector2 *buttonPos, int amount, int turn, int hovered)
{
	Vector2 border;
	Vector2 pos;
	Vector2 texturePos;
	float gap, center;
	int menuHeight;
	Texture2D *texture;
	double scale = 0.2;
	Color tileCol = turn % 2 ? player2Color : player1Color;

	gap = winDimensions.x / amount;
	center = winDimensions.x / 2;

	menuHeight = HEX_RADIUS*2 + 20;

	if (gap > HEX_RADIUS * 2)
		gap = HEX_RADIUS * 2;

	border.y = 10;

	pos.y = winDimensions.y - HEX_RADIUS - border.y;

	DrawRectangleRec((Rectangle) {0, winDimensions.y - menuHeight, winDimensions.x, menuHeight}, menuBgColor);
	DrawLineEx((Vector2) {0, winDimensions.y - menuHeight}, (Vector2) {winDimensions.x, winDimensions.y - menuHeight}, 8, menuBorderColor);

	for (int i = 0; i < amount; i++) {
		if (i < amount/2)
			pos.x = center - (gap * (i+1));
		if (i == amount/2)
			pos.x = center;
		if (i > amount/2)
			pos.x = center + (gap * (i-2));

		DrawPoly(pos, 6, HEX_RADIUS, 0, tileCol);
		DrawPolyLinesEx(pos, 6, HEX_RADIUS, 0, 10, DARKGRAY);

		if (i == hovered)
			DrawPolyLinesEx(pos, 6, HEX_RADIUS, 0, 10, BLUE);

		switch (i)
		{
		case 0:
			texture = &soldierAntTexture;
			break;
		case 1:
			texture = &beeTexture;
			break;
		case 2:
			texture = &beetleTexture;
			break;
		case 3:
			texture = &grasshopperTexture;
			break;
		case 4:
			texture = &spiderTexture;
			break;
		}

		texturePos = pos;
		texturePos.x -= ((texture->width	* scale)/2);
		texturePos.y -= ((texture->height * scale)/2);

		DrawTextureEx(*texture, texturePos, 0, scale, WHITE);

		buttonPos[i] = pos;
	}
}

void DrawTurnBox(int turn)
{
	int textlen = MeasureText(TextFormat("Turn: %d", turn+1), 30);
	int ringOffset = turn % 2 ? 65 : 25;

	DrawRectangleRounded((Rectangle) {20, 20, textlen + 70, 40}, 0.2, 0, RED);
	DrawRectangleRoundedLines((Rectangle) {21, 21, textlen + 68, 38}, 0.2, 0, 3, BLACK);

	DrawText(TextFormat("Turn: %d", turn+1), 30, 30, 20, BLACK);

	DrawCircle(textlen + 25, 40, 15, player1Color);
	DrawCircle(textlen + 65, 40, 15, player2Color);
	DrawRing((Vector2) {textlen+ringOffset,40}, 12, 16, 0, 360, 0, BLUE);
}
