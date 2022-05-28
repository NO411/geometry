#include "math_misc.h"
#include "../app/geometry_objects.h"
#include "raylib.h"

#include <math.h>

Camera2D camera = {0};

float GetMouseX2()
{
	return (GetMouseX() - camera.offset.x + GetScreenWidth() / 2);
}

float GetMouseY2()
{
	return (GetMouseY() - camera.offset.y + GetScreenHeight() / 2);
}

Vector2 *GetMousePosition2()
{
	return new Vector2{GetMouseX2(), GetMouseY2()};
}

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

Vector2 *GetOrthogonalLinesIntersection(Vector2 *point, Vector2 *pointA, Vector2 *pointB)
{
	Vector2 mn = *GetMN(pointA, pointB);
	float m2 = -1 / mn.x;
	float n2 = point->y - m2 * point->x;
	float x = (n2 - mn.y) / (mn.x - m2);
	float y = mn.x * x + mn.y;

	Vector2 connectionPoint = {x, y};

	if (pointA->x == pointB->x)
	{
		connectionPoint = {pointA->x, point->y};
	}

	if (pointA->y == pointB->y)
	{
		connectionPoint = {point->x, pointA->y};
	}

	return new Vector2{connectionPoint};
}

Vector2 *GetCircleConnection(Circle *circle)
{
	Vector2 *mouse = GetMousePosition2();
	float x = circle->middle.x + (mouse->x - circle->middle.x) / (GetDistance(mouse, &circle->middle) / circle->radius);
	Vector2 mn = *GetMN(&circle->middle, mouse);
	float y = mn.x * x + mn.y;

	return new Vector2{x, y};
}

Vector2 *CalculateConnectionPoint(Vector2 *p1, Vector2 *p2, float m, float n)
{
	Vector2 connectionPoint;

	// correct connectionPoint offset
	Vector2 offset = {
		camera.offset.x - GetScreenWidth() / 2,
		camera.offset.y - GetScreenHeight() / 2};

	if (p1->x > p2->x)
	{
		connectionPoint.x = -offset.x;
	}
	else
	{
		connectionPoint.x = GetScreenWidth() - offset.x;
	}

	connectionPoint.y = m * connectionPoint.x + n;

	if (p1->x == p2->x)
	{
		connectionPoint.x = p1->x;
		if (p1->y > p2->y)
		{
			connectionPoint.y = -offset.y;
		}
		else
		{
			connectionPoint.y = GetScreenHeight() - offset.y;
		}
	}

	return new Vector2{connectionPoint};
}