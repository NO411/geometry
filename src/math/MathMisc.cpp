#include "MathMisc.h"
#include "raylib.h"
#include <cmath>

Vector2 GetMousePosition2(Camera2D &camera)
{
	return GetScreenToWorld2D(GetMousePosition(), camera);
}

float GetDistance(Vector2 &vec1, Vector2 &vec2)
{
	return sqrt(pow(vec2.x - vec1.x, 2) + pow(vec2.y - vec1.y, 2));
}
