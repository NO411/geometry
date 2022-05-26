#include "raylib.h"
#include "math_misc.h"
#include <math.h>

Vector2 *GetMN(Vector2 *p1, Vector2 *p2)
{
    float m = (p1->y - p2->y) / (p1->x - p2->x);
    float n = p2->y - (m * p2->x);
    return new Vector2{m, n};
}

float GetDistance(Vector2 *vec1, Vector2 *vec2)
{
    return sqrt(pow(vec2->x - vec1->x, 2) + pow(vec2->y - vec1->y, 2));
}

bool SameVector2(Vector2 *v1, Vector2 *v2)
{
	return (v1->x == v2->x && v1->y == v2->y);
}

bool IsPointOnLine(Vector2 *point, Vector2 *pointA, Vector2 *pointB)
{
	float x1 = pointA->x, x2 = pointB->x, _var;
	if (x1 > x2)
	{
		_var = x1;
		x1 = x2;
		x2 = _var;
	}
	if (point->x >= x1 && point->x <= x2)
	{
		return true;
	}
	if (x1 == x2)
	{
		float y1 = pointA->y, y2 = pointB->y;
		if (y1 > y2)
		{
			_var = y1;
			y1 = y2;
			y2 = _var;
		}
		if (point->y >= y1 && point->y <= y2)
		{
			return true;
		}
	}
	return false;
}
