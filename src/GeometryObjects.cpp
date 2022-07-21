#include "GeometryObjetcs.h"
#include "raylib.h"
#include "math/MathMisc.h"

const Color GemObj::renderColor = GRAY;
const int GemObj::renderThickness = 2;

Distance::Distance(Vector2 &pointA_, Vector2 &pointB_)
{
    pointA = pointA_;
    pointB = pointB_;
}

void Distance::Render(Camera2D &camera)
{
    DrawLineEx(GetWorldToScreen2D(pointA, camera), GetWorldToScreen2D(pointB, camera), renderThickness, renderColor);
}

StraightLine::StraightLine(Vector2 &pointA_, Vector2 &pointB_, Camera2D &camera)
{
    pointA = pointA_;
    pointB = pointB_;
    UpdateDrawPoints(camera);
}

void StraightLine::UpdateDrawPoints(Camera2D &camera)
{
    LinearFunction f = GetLinearFunction(pointA, pointB);
    firstDrawPoint = CalculateConnectionPoint(pointA, pointB, f.m, f.n, camera);
    secondDrawPoint = CalculateConnectionPoint(pointB, pointA, f.m, f.n, camera);
}

void StraightLine::Render()
{
    DrawLineEx(firstDrawPoint, secondDrawPoint, renderThickness, renderColor);
}

Ray2::Ray2(Vector2 &pointA_, Vector2 &pointB_, Camera2D &camera)
{
    pointA = pointA_;
    pointB = pointB_;
    UpdateDrawPoint(camera);
}

void Ray2::UpdateDrawPoint(Camera2D &camera)
{
    LinearFunction f = GetLinearFunction(pointA, pointB);
    drawPoint = CalculateConnectionPoint(pointA, pointB, f.m, f.n, camera);
}

void Ray2::Render(Camera2D &camera)
{
    DrawLineEx(GetWorldToScreen2D(pointA, camera), drawPoint, renderThickness, renderColor);
}

Circle::Circle(Vector2 &center, float radius) : center(center), radius(radius)
{
}

void Circle::Render(Camera2D &camera)
{
    DrawRing(GetWorldToScreen2D(center, camera), radius * camera.zoom - renderThickness / 2, radius * camera.zoom + renderThickness / 2, 0, 360, 200, renderColor);
}
