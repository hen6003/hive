#include <stdlib.h>
#include <math.h>

#include "collision.h"
#include "poly.h"

extern VectorTile Board_Size;
extern bool OffsetInitialRow;

bool CheckCollisionPointHexGrid(Vector2 point, VectorTile gridPos)
{
	bool ret;
	Vector2 realPos;
	int size = HEX_RADIUS;

	realPos.x = gridPos.x * (size * 1.66);
	realPos.y = gridPos.y * (size * 1.434);

	if (abs(gridPos.y) % 2 == !OffsetInitialRow)
		realPos.x += size * HEX_OFFSET;

	ret = CheckCollisionPointPoly(point, realPos, 6, size);

	return ret;
}

VectorTile GetCollisionHexGrid(Vector2 point)
{
	for (int x = 0; x < Board_Size.x; x++)
		for (int y = 0; y < Board_Size.y; y++)
			if (CheckCollisionPointHexGrid(point, (VectorTile) {x,y}))
				return (VectorTile) {x,y};

	return (VectorTile) {Board_Size.x+1,Board_Size.y+1};
}
