#pragma once

#include "GeometryObjetcs.h"
#include "raylib.h"
#include <vector>
#include <unordered_map>
#include <tuple>

enum EditMode
{
	DRAW_CIRCLE,
	DRAW_DISTANCE,
	DRAW_RAY,
	DRAW_STRAIGHT_LINE,
	DRAW_POINT,
	ERASER,
	LENGTH_MEASUREMENT,
	ANGLE_MEASUREMENT,
	LENGTH_MEASUREMENT_ERASER,
	ANGLE_MEASUREMENT_ERASER,
	CIRCLE_ERASER,
	MOVE_OBJECT,
};

class IntersectionStorage
{
public:
	std::unordered_map<int, Vec2> intersections;
	int currentID = 0;

	int Push(Vec2 &newIntersection);
};

class GeometryBoard;

class SelectedThing
{
public:
	SelectedThing();
	SelectedThing(std::size_t objPos, int objType, Vec2 &originalPoint, GeometryBoard *board_);

	std::size_t objPos;
	int objType;

	Vec2 originalPoint;
	int memberObj;

private:
	GeometryBoard *board_;

	template <typename L>
	void SetUpLineMemberObjNumber(std::vector<L> &objVector);
};

class GeometryApp;

class GeometryBoard
{
	friend Point;
	friend SelectedThing;

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
	Vec2 firstPoint;
	Point currentPoint;
	Vec2 firstCircleEraserPos;
	std::size_t currentObjPos;

	std::vector<Circle> circles;
	std::vector<Distance> distances;
	std::vector<Ray2> rays;
	std::vector<StraightLine> straightLines;
	std::vector<Point> points;

	SelectedThing selectedThing;
	void UpdateMovingObj();

	template <typename T>
	void EraseGemObj(std::vector<T> &objVec, std::size_t vecPos, bool eraseFromVector);
	void EraseObj(int objType, std::size_t objPos, bool eraseFromVector);
	void ErasePoint(std::size_t vecPos);

	IntersectionStorage intersections;

	void AddIntersections(Circle &circle);
	template <typename L>
	void AddIntersections(L &line);

	void CheckResized();
	void InputHandler();
	void Edit();
	std::tuple<int, std::size_t> UpdateCurrentPoint();
	void SetEditMode();
	void InterruptDrawing();
	void ModifyViewField();
	void DrawDrawingObj();
	void UpdateConnectionDistance();
	bool PointLetterExists(std::string &letter);

	template <typename O>
	void Measure(O &object);

	void DrawCurrentCircleSector();

	GeometryApp *app_;
};
