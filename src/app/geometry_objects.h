#ifndef GEOMETRY_OBJECTS_H
#define GEOMETRY_OBJECTS_H

#include "raylib.h"

#include <string>

enum GeometryObject
{
	CIRCLE,
	DISTANCE,
	RAY,
	STRAIGHTLINE,
	POINT
};

class GeometryObj
{
public:
	int objectNumber;

	GeometryObj();
	~GeometryObj();

	void CheckIntersections(Vector2 *vec1, Vector2 *vec2, float r);
};

class Point
{
public:
	Vector2 point;
	std::string letter;
	std::string letterNumber;

	Point();
	Point(Vector2 point);
	~Point();

	void SetPointLetter();
	void Move(int direction, bool y);
};

class Circle : public GeometryObj
{
public:
	Vector2 middle;
	float radius;

	Circle(Vector2 middle, float radius);
	~Circle();

	void UpdateIntersections();
	void Move(int direction, bool y);
};

class Line : public GeometryObj
{
public:
	Vector2 pointA;
	Vector2 pointB;
	Vector2 firstConnectionPoint;
	Vector2 secondConnectionPoint;
	bool showLength = false;
	float length;
	Point middle;

	Line(Vector2 pointA, Vector2 pointB, int n);
	~Line();

	void UpdateFirstConnectionPoint();
	void UpdateSecondConnectionPoint();
	void UpdateConnectionPoints();
	void UpdateIntersections();
	void Move(int direction, bool y);
	void UpdateLength();
};

#endif
