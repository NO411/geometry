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
	Distance();

	void Render();
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
