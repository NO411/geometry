#pragma once

#include "raylib.h"
#include <string>
#include <vector>

enum GeometryObject
{
	CIRCLE,
	DISTANCE,
	RAY,
	STRAIGHTLINE,
	POINT
};

class GeometryBoard;

class GemObj
{
public:
	static const Color renderColor;
	static const int renderThickness;

	void DrawLineExSmooth(Vector2 &startPos, Vector2 &endPos);
	void DrawRingSmooth(Vector2 &center, float radius);

	std::vector<int> intersectionIDs;
};

class Circle : public GemObj
{
public:
	Circle(Vector2 &center, float radius);

	void Render(Camera2D &camera);

	Vector2 center;
	float radius;
};

class Line : public GemObj
{
public:
	Vector2 pointA, pointB;

	bool IsVerticalLine();
};

class Distance : public Line
{
public:
	Distance(Vector2 &pointA_, Vector2 &pointB_);

	void Render(Camera2D &camera);

	bool IsPointOnLine(Vector2 &point);

private:
};

class Ray2 : public Line
{
public:
	Ray2(Vector2 &pointA_, Vector2 &pointB_, Camera2D &camera);

	void Render(Camera2D &camera);
	void UpdateDrawPoint(Camera2D &camera);

	bool IsPointOnLine(Vector2 &point);

private:
	Vector2 drawPoint;
};

class StraightLine : public Line
{
public:
	StraightLine(Vector2 &pointA_, Vector2 &pointB_, Camera2D &camera);

	void Render();
	void UpdateDrawPoints(Camera2D &camera);

	bool IsPointOnLine(Vector2 &point);

private:
	Vector2 firstDrawPoint, secondDrawPoint;
};

class Point : public GemObj
{
public:
	Point();
	Point(Vector2 &pos, GeometryBoard *board);

	void Render(Camera2D &camera);
	void Render(Camera2D &camera, Font &font);

	Vector2 &GetPos();
	std::string &GetLetter();
	void SetPos(Vector2 &pos);

private:
	void SetPointLetter(GeometryBoard *board);

	Vector2 point;
	std::string letter;
	std::string letterNumber;

	static const std::string pointChars;
};
