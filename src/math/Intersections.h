#pragma once

#include "../GeometryObjetcs.h"
#include "raylib.h"
#include <vector>

std::vector<Vector2> GetCircleCircleIntersection(Vector2 &center, float radius, Vector2 &center_, float radius_);
std::vector<Vector2> GetLineCircleIntersection(Vector2 &pointA, Vector2 &pointB, Vector2 &center, float radius);
std::vector<Vector2> GetLineLineIntersection(Vector2 &pointA, Vector2 &pointB, Vector2 &pointA_, Vector2 &pointB_);
bool IsPointOnDistance(Vector2 &intersection, Distance &distance);
bool IsPointOnRay(Vector2 &intersection, Ray2 &ray);
void GetStraightLineCircleIntersections(std::vector<Vector2> &intersections, StraightLine &straightLine, Circle &circle);
void GetRayCircleIntersections(std::vector<Vector2> &intersections, Ray2 &ray, Circle &circle);
void GetCircleCircleIntersections(std::vector<Vector2> &intersections, Circle &circle1, Circle &circle2);
void GetDistanceDistanceIntersections(std::vector<Vector2> &intersections, Distance &distance1, Distance &distance2);
void GetStraightLineDistanceIntersections(std::vector<Vector2> &intersections, StraightLine &straightLine, Distance &distance);
void GetRayDistanceIntersections(std::vector<Vector2> &intersections, Ray2 &ray, Distance &distance);
void GetDistanceCircleIntersections(std::vector<Vector2> &intersections, Distance &distance, Circle &circle);
void GetStraightLineRayIntersections(std::vector<Vector2> &intersections, StraightLine &straightLine, Ray2 &ray);
void GetRayRayIntersections(std::vector<Vector2> &intersections, Ray2 &ray1, Ray2 &ray2);
void GetStraightLineStraightLinieIntersections(std::vector<Vector2> &intersections, StraightLine &straightLine1, StraightLine &straightLine2);
