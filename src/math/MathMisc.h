#pragma once

#include "raylib.h"

class LinearFunction
{
public:
    LinearFunction(Vector2 &pointA, Vector2 &pointB);

    float m;
    float n;
    bool vertical;
};

Vector2 GetMousePosition2(Camera2D &camera);
float GetDistance(Vector2 &vec1, Vector2 &vec2);
bool SameFloat(float a, float b);
Vector2 CalculateConnectionPoint(Vector2 &p1, Vector2 &p2, float m, float n, Camera2D &camera);
