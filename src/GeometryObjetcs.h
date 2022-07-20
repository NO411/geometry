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

class GemObj
{
public:
	static const Color renderColor;
	static const int renderThickness;
};

class Circle : public GemObj
{
public:
	Circle();

	void Render();

private:
	Vector2 center;
	float radius;
};

class Line : public GemObj
{
public:
	Vector2 pointA, pointB;
};

class Distance : public Line
{
public:
	Distance(Vector2 &pointA_, Vector2 &pointB_);

	void Render(Camera2D &camera);

private:
};

class Ray2 : public Line
{
public:
	Ray2(Vector2 &pointA_, Vector2 &pointB_, Camera2D &camera);

	void Render(Camera2D &camera);
	void UpdateDrawPoint(Camera2D &camera);
private:
	Vector2 drawPoint;
};

class StraightLine : public Line
{
public:
	StraightLine(Vector2 &pointA_, Vector2 &pointB_, Camera2D &camera);

	void Render();
	void UpdateDrawPoints(Camera2D &camera);

private:
	Vector2 firstDrawPoint, secondDrawPoint;
};

class Point : public GemObj
{
public:
	Point();

	void Render();

protected:
	Vector2 point;
};
