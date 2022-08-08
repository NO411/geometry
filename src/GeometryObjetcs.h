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

class Vec2
{
public:
	Vec2(long double x, long double y);
	Vec2(Vector2 &v);
	Vec2(Vector2 v);
	Vec2();
	long double x;
	long double y;

	bool operator==(Vec2 &v);

	Vector2 ToRaylibVec();
};

class GemObj
{
public:
	static const Color renderColor;
	static const int renderThickness;
	static const long double LE_factor;

	void DrawLineExSmooth(Vec2 &startPos, Vec2 &endPos);
	void DrawRingSmooth(Vec2 &center, long double radius);

	std::vector<int> intersectionIDs;
};

class LengthMeasurement
{
public:
	void EnableLength(Vec2 &pos, Font *font);
	void DisableLength();
	bool IsLengthEnabled();

	void RenderLength(Camera2D &camera);

	bool showLength = false;
	Vec2 lengthPos;
	std::string length;
	Vector2 measure;

	Font *boardFont;
	static const int fontSize;
};

class Circle : public GemObj, public LengthMeasurement
{
public:
	Circle(Vec2 &center, long double radius);

	void Render(Camera2D &camera);

	Vec2 center;
	long double radius;
	void UpdateLength();
};

class Line : public GemObj
{
public:
	Vec2 pointA, pointB;

	bool IsVerticalLine();
};

class Distance : public Line, public LengthMeasurement
{
public:
	Distance(Vec2 &pointA_, Vec2 &pointB_);

	void Render(Camera2D &camera);

	bool IsPointOnLine(Vec2 &point);
	void UpdateLength();
};

class Ray2 : public Line
{
public:
	Ray2(Vec2 &pointA_, Vec2 &pointB_, Camera2D &camera);

	void Render(Camera2D &camera);
	void UpdateDrawPoint(Camera2D &camera);

	bool IsPointOnLine(Vec2 &point);

private:
	Vec2 drawPoint;
};

class StraightLine : public Line
{
public:
	StraightLine(Vec2 &pointA_, Vec2 &pointB_, Camera2D &camera);

	void Render();
	void UpdateDrawPoints(Camera2D &camera);

	bool IsPointOnLine(Vec2 &point);

private:
	Vec2 firstDrawPoint, secondDrawPoint;
};

class Point : public GemObj
{
public:
	Point();
	Point(Vec2 &pos, GeometryBoard *board);

	void Render(Camera2D &camera);
	void Render(Camera2D &camera, Font &font);

	Vec2 &GetPos();
	std::string &GetLetter();
	void SetPos(Vec2 &pos);

private:
	void SetPointLetter(GeometryBoard *board);

	Vec2 point;
	std::string letter;
	std::string letterNumber;

	static const std::string pointChars;
};
