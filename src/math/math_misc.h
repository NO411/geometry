#ifndef MATH_MISC_H
#define MATH_MISC_H

#include "raylib.h"

Vector2 *GetMN(Vector2 *p1, Vector2 *p2);
float GetDistance(Vector2 *vec1, Vector2 *vec2);
bool SameVector2(Vector2 *v1, Vector2 *v2);
bool IsPointOnLine(Vector2 *point, Vector2 *pointA, Vector2 *pointB);

#endif
