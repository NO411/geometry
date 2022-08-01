#pragma once

#include "GeometryObjetcs.h"
#include "raylib.h"
#include <vector>
#include <unordered_map>

enum EditMode
{
	DRAW_CIRCLE,
	DRAW_DISTANCE,
	DRAW_RAY,
	DRAW_STRAIGHT_LINE,
	DRAW_POINT,
	ERASER,
	DISTANCE_MEASUREMENT,
	ANGLE_MEASUREMENT,
	DISTANCE_MEASUREMENT_ERASER,
	ANGLE_MEASUREMENT_ERASER,
	CIRCLE_ERASER,
};

class IntersectionStorage
{
public:
	std::unordered_map<int, Vector2> intersections;
	int currentID = 0;

	int Push(Vector2 &newIntersection);
};

class GeometryApp;

class GeometryBoard
{
	friend Point;
public:
	GeometryBoard();
	GeometryBoard(GeometryApp *app);
	~GeometryBoard();

	void Render();
	void Update();

	void UpdateAllDrawPoints();
private:
	int editMode = DRAW_DISTANCE;
	Camera2D camera;

	static const float movementSpeed;
	static const float minZoom;
	static const float maxZoom;
	static const int connectionDistance_;
	static int connectionDistance;

	bool firstPointed = false;
	Vector2 firstPoint;
	Point currentPoint;

	std::vector<Circle> circles;
	std::vector<Distance> distances;
	std::vector<Ray2> rays;
	std::vector<StraightLine> straightLines;
	std::vector<Point> points;

	template<typename T>
	void EraseGemObj(std::vector<T> &objVec, std::size_t vecPos);
	void ErasePoint(std::size_t vecPos);

	IntersectionStorage intersections;
	
	void AddIntersections(Circle &circle);
	template<typename L>
	void AddIntersections(L &line);

	void CheckResized();
	void InputHandler();
	void Edit();
	void UpdateCurrentPoint();
	void SetEditMode();
	void InterruptDrawing();
	void ModifyViewField();
	void DrawDrawingObj();
	void UpdateConnectionDistance();
	bool PointLetterExists(std::string &letter);

	GeometryApp *app_;
};
