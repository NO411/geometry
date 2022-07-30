#include "MathMisc.h"
#include "raylib.h"
#include <cmath>

LinearFunction::LinearFunction(Vector2 &p1, Vector2 &p2)
{
	m = (p1.y - p2.y) / (p1.x - p2.x);
	n = p2.y - (m * p2.x);
	vertical = SameFloat(p1.x, p2.x);
}

Vector2 GetMousePosition2(Camera2D &camera)
{
	return GetScreenToWorld2D(GetMousePosition(), camera);
}

float GetDistance(Vector2 &vec1, Vector2 &vec2)
{
	return sqrt(pow(vec2.x - vec1.x, 2) + pow(vec2.y - vec1.y, 2));
}

bool SameFloat(float a, float b)
{
	return std::abs(a - b) <= __FLT_EPSILON__ * std::max(std::abs(a), std::abs(b));
}

Vector2 CalculateConnectionPoint(Vector2 &p1, Vector2 &p2, float m, float n, Camera2D &camera)
{
	Vector2 connectionPoint;

	if (p1.x > p2.x)
	{
		connectionPoint.x = GetScreenToWorld2D({0, 0}, camera).x;
	}
	else
	{
		connectionPoint.x = GetScreenToWorld2D({(float)GetScreenWidth(), 0}, camera).x;
	}

	connectionPoint.y = m * connectionPoint.x + n;

	if (SameFloat(p1.x, p2.x))
	{
		connectionPoint.x = p1.x;
		if (p1.y > p2.y)
		{
			connectionPoint.y = GetScreenToWorld2D({0, 0}, camera).y;
		}
		else
		{
			connectionPoint.y = GetScreenToWorld2D({0, (float)GetScreenHeight()}, camera).y;
		}
	}

	return GetWorldToScreen2D(Vector2{connectionPoint}, camera);
}
