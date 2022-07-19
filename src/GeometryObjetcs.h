#pragma once

#include "raylib.h"

enum GeometryObject
{
	CIRCLE,
	DISTANCE,
	RAY,
	STRAIGHTLINE,
	POINT
};

class Circle
{
public:
	Circle();

	void Render();

private:
	Vector2 center;
	float radius;
};

class Line
{
private:
	Vector2 pointA, pointB;
};

class Distance
{
public:
	Distance(Vector2 startPos, Vector2 endPos);

	void Render(Camera2D &camera);
private:
	Vector2 startPos, endPos;
};

class Ray2
{
public:
	Ray2();

	void Render();
};

class StraightLine
{
public:
	StraightLine();

	void Render();
};

class Point
{
public:
	Point();

	void Render();

protected:
	Vector2 point;
};
