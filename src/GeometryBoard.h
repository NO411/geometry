#pragma once

#include "GeometryObjetcs.h"
#include "raylib.h"
#include <vector>

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

class GeometryApp;

class GeometryBoard
{
public:
	GeometryBoard();
	GeometryBoard(GeometryApp *app);
	~GeometryBoard();

	void Render();
	void Update();
private:
	int editMode = DRAW_DISTANCE;
	Camera2D camera;

	static const float movementSpeed;
	static const float minZoom;
	static const float maxZoom;
	static const int connectionDistance;

	bool firstPointed = false;
	Vector2 firstPoint;
	Vector2 currentPoint;

	std::vector<Circle> circles;
	std::vector<Distance> distances;
	std::vector<Ray2> rays;
	std::vector<StraightLine> straightLines;
	std::vector<Point> points;
	std::vector<Vector2> intersections;

	void CheckResized();
	void InputHandler();
	void Edit();
	void UpdateCurrentPoint();
	void SetEditMode();
	void InterruptDrawing();
	void ModifyViewField();

	GeometryApp *app_;
};
