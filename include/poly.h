#include <stdbool.h>
#include <raylib.h>

bool CheckCollisionPointPolyEx(Vector2 point, Vector2 *polygon, int vertexCount);
bool CheckCollisionPointPoly(Vector2 point, Vector2 center, int sides, float radius);
