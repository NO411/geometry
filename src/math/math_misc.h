#ifndef MATH_MISC_H
#define MATH_MISC_H

#include "../app/geometry_objects.h"
#include "raylib.h"

extern Camera2D camera;

float GetMouseX2();
float GetMouseY2();
Vector2 *GetMousePosition2();
Vector2 *GetMN(Vector2 *p1, Vector2 *p2);
float GetDistance(Vector2 *vec1, Vector2 *vec2);
bool IsPointOnLine(Vector2 *point, Vector2 *pointA, Vector2 *pointB);
Vector2 *GetOrthogonalLinesIntersection(Vector2 *point, Vector2 *pointA, Vector2 *pointB);
Vector2 *GetCircleConnection(Circle *circle);
Vector2 *CalculateConnectionPoint(Vector2 *p1, Vector2 *p2, float m, float n);

#endif
