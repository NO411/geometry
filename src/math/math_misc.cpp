#include "raylib.h"
#include "math_misc.h"

Vector2 *GetMN(Vector2 *p1, Vector2 *p2)
{
	float m = (p1->y - p2->y) / (p1->x - p2->x);
	float n = p2->y - (m * p2->x);
	return new Vector2{m, n};
}