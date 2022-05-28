#ifndef INTERSECTIONS_H
#define INTERSECTIONS_H

#include "../app/geometry_objects.h"
#include "raylib.h"

#include <vector>

extern std::vector<Point> intersections;

void FindCircleCircleIntersections(Vector2 *A, Vector2 *B, float a, float b);
void FindLineLineIntersections(Vector2 *A1, Vector2 *A2, Vector2 *B1, Vector2 *B2);
void FindCircleLineIntersections(Vector2 *A, float r, Vector2 *pointA, Vector2 *pointB);

#endif
