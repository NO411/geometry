#pragma once

#include "raylib.h"

struct LinearFunction
{
    float m;
    float n;
};

Vector2 GetMousePosition2(Camera2D &camera);
float GetDistance(Vector2 &vec1, Vector2 &vec2);
bool SameFloat(float a, float b);
LinearFunction GetLinearFunction(Vector2 &p1, Vector2 &p2);
Vector2 CalculateConnectionPoint(Vector2 &p1, Vector2 &p2, float m, float n, Camera2D &camera);
