#include "GeometryBoard.h"
#include "GeometryApp.h"
#include "GeometryObjetcs.h"
#include "math/MathMisc.h"
#include "math/Intersections.h"
#include "raylib.h"
#include <tuple>
#include <algorithm>

const float GeometryBoard::movementSpeed = 6;
const float GeometryBoard::minZoom = 0.125;
const float GeometryBoard::maxZoom = 6;
const int GeometryBoard::connectionDistance_ = 8;
int GeometryBoard::connectionDistance = connectionDistance_;

int IntersectionStorage::Push(Vec2 &newIntersection)
{
	currentID++;
	intersections[currentID] = newIntersection;
	return currentID;
}

GeometryBoard::GeometryBoard() {}

GeometryBoard::GeometryBoard(GeometryApp *app) : app_(app)
{
	camera = {0};
	camera.zoom = 1;
}

GeometryBoard::~GeometryBoard() {}

void GeometryBoard::Render()
{
	bool renderMovingPoints = (editMode == MOVE_OBJECT);
	for (auto &distance : distances)
	{
		distance.Render(camera, renderMovingPoints);
	}
	for (auto &straightLine : straightLines)
	{
		straightLine.Render(camera, renderMovingPoints);
	}
	for (auto &ray : rays)
	{
		ray.Render(camera, renderMovingPoints);
	}
	for (auto &circle : circles)
	{
		circle.Render(camera, renderMovingPoints);
	}
	for (auto &point : points)
	{
		point.Render(camera, app_->font);
	}

	/*// Render intersections
	for (auto &intersection : intersections.intersections)
	{
		Point point(intersection.second, this);
		point.Render(camera);
	}*/

	DrawDrawingObj();
	currentPoint.Render(camera);

	BeginMode2D(camera);
	EndMode2D();
}

void GeometryBoard::DrawDrawingObj()
{
	if (!firstPointed)
	{
		return;
	}

	Vec2 currentPos = currentPoint.GetPos();

	if (currentPos == firstPoint)
	{
		return;
	}

	switch (editMode)
	{
	case DRAW_CIRCLE:
	{
		Circle currentCircle(firstPoint, currentPos);
		currentCircle.Render(camera, false);
	}
	break;
	case DRAW_DISTANCE:
	{
		Distance currentDistance(firstPoint, currentPos);
		currentDistance.Render(camera, false);
	}
	break;
	case DRAW_RAY:
	{
		Ray2 currentRay(firstPoint, currentPos, camera);
		currentRay.Render(camera, false);
	}
	break;
	case DRAW_STRAIGHT_LINE:
	{
		StraightLine currentStraightLine(firstPoint, currentPos, camera);
		currentStraightLine.Render(camera, false);
	}
	break;
	case CIRCLE_ERASER:
	{
		DrawCurrentCircleSector();
	}
	break;
	default:
		break;
	}
}

void GeometryBoard::DrawCurrentCircleSector()
{
	Circle circle = circles[currentObjPos];
	Color sectorColor = GRAY;

	if (IsPointOnCircle(currentPoint.GetPos(), circle))
	{
		sectorColor = GREEN;
	}

	sectorColor.a = 100;
	float startAngle = CirclePointToAngle(circle.center, firstCircleEraserPos);
	float endAngle = CirclePointToAngle(circle.center, currentPoint.GetPos());

	Vector2 center_ = GetWorldToScreen2D(circle.center.ToRaylibVec(), camera);

	if (startAngle > endAngle)
	{
		DrawCircleSector(center_, circle.radius * camera.zoom, startAngle, 360, 500, sectorColor);
		DrawCircleSector(center_, circle.radius * camera.zoom, 0, endAngle, 500, sectorColor);
		return;
	}

	DrawCircleSector(center_, circle.radius * camera.zoom, startAngle, endAngle, 500, sectorColor);
}

void GeometryBoard::Update()
{
	InputHandler();
	CheckResized();
}

void GeometryBoard::UpdateConnectionDistance()
{
	connectionDistance = 8 / camera.zoom;
}

void GeometryBoard::CheckResized()
{
	if (!IsWindowResized())
	{
		return;
	}

	UpdateAllDrawPoints();
	app_->helpWindow.scrollBar.UpdateOnResize();
}

void GeometryBoard::InputHandler()
{
	Edit();
	SetEditMode();
	InterruptDrawing();
	ModifyViewField();
}

template <typename O>
void GeometryBoard::Measure(O &object)
{
	object.EnableLength(currentPoint.GetPos(), &app_->font);
	object.UpdateLength();
}

void GeometryBoard::UpdateMovingObj()
{
	switch (selectedThing.objType)
	{
	case CIRCLE:
		if (selectedThing.memberObj < 2)
		{
			circles[selectedThing.objPos].MovePos(selectedThing.originalPoint, currentPoint.GetPos());
		}
		else
		{
			circles[selectedThing.objPos].MoveRadius(selectedThing.originalPoint, currentPoint.GetPos());
		}
		break;
	case DISTANCE:
		if (selectedThing.memberObj == 0)
		{
			distances[selectedThing.objPos].MovePos(selectedThing.originalPoint, currentPoint.GetPos());
		}
		else
		{
			distances[selectedThing.objPos].MovePoint(selectedThing.originalPoint, currentPoint.GetPos(), selectedThing.memberObj);
		}
		break;
	case RAY:
		if (selectedThing.memberObj == 0)
		{
			rays[selectedThing.objPos].MovePos(selectedThing.originalPoint, currentPoint.GetPos(), camera);
		}
		else
		{
			rays[selectedThing.objPos].MovePoint(selectedThing.originalPoint, currentPoint.GetPos(), selectedThing.memberObj, camera);
		}
		break;
	case STRAIGHTLINE:
		if (selectedThing.memberObj == 0)
		{
			straightLines[selectedThing.objPos].MovePos(selectedThing.originalPoint, currentPoint.GetPos(), camera);
		}
		else
		{
			straightLines[selectedThing.objPos].MovePoint(selectedThing.originalPoint, currentPoint.GetPos(), selectedThing.memberObj, camera);
		}
		break;
	case POINT:
		points[selectedThing.objPos].SetPos(currentPoint.GetPos());
		break;

	default:
		break;
	}

	selectedThing.originalPoint = currentPoint.GetPos();
}

void GeometryBoard::Edit()
{
	auto objTuple = UpdateCurrentPoint();
	int objType = std::get<0>(objTuple);
	std::size_t objPos = std::get<1>(objTuple);

	if (editMode == MOVE_OBJECT && firstPointed)
	{
		UpdateMovingObj();
	}

	if (!(IsMouseButtonPressed(0) || IsKeyPressed(KEY_ENTER)) || app_->button.Selected())
	{
		return;
	}

	if (!firstPointed)
	{
		firstPointed = true;
		firstPoint = currentPoint.GetPos();

		switch (editMode)
		{
		case DRAW_POINT:
			firstPointed = false;
			for (auto &point : points)
			{
				if (GetDistance(point.GetPos(), currentPoint.GetPos()) < connectionDistance)
				{
					return;
				}
			}
			points.push_back(Point{currentPoint.GetPos(), this});
			break;
		case ERASER:
			firstPointed = false;
			EraseObj(objType, objPos, true);
			break;
		case LENGTH_MEASUREMENT:
			firstPointed = false;
			switch (objType)
			{
			case DISTANCE:
				Measure(distances[objPos]);
				distances[objPos].SetLengthPointPosition();
				break;
			case CIRCLE:
				Measure(circles[objPos]);
				break;

			default:
				break;
			}
			break;
		case ANGLE_MEASUREMENT:
			firstPointed = false;
			break;
		case LENGTH_MEASUREMENT_ERASER:
			firstPointed = false;
			switch (objType)
			{
			case DISTANCE:
				distances[objPos].DisableLength();
				break;
			case CIRCLE:
				circles[objPos].DisableLength();
				break;

			default:
				break;
			}
			break;
		case ANGLE_MEASUREMENT_ERASER:
			firstPointed = false;
			break;
		case CIRCLE_ERASER:
			if (objType != CIRCLE)
			{
				firstPointed = false;
				break;
			}

			firstCircleEraserPos = firstPoint;
			currentObjPos = objPos;
			break;
		case MOVE_OBJECT:
			if (objType < CIRCLE)
			{
				firstPointed = false;
				break;
			}

			EraseObj(objType, objPos, false);
			selectedThing = {objPos, objType, firstPoint, this};

			break;
		default:
			break;
		}
	}
	else
	{
		// operations which need two mouse clicks

		firstPointed = false;
		Vec2 currentPos = currentPoint.GetPos();

		if (firstPoint == currentPos)
		{
			return;
		}

		switch (editMode)
		{
		case DRAW_CIRCLE:
		{
			circles.emplace_back(firstPoint, currentPos);
			AddIntersections(circles.back());
		}
		break;
		case DRAW_DISTANCE:
		{
			distances.emplace_back(firstPoint, currentPos);
			AddIntersections(distances.back());
		}
		break;
		case DRAW_RAY:
		{
			rays.emplace_back(firstPoint, currentPos, camera);
			AddIntersections(rays.back());
		}
		break;
		case DRAW_STRAIGHT_LINE:
		{
			straightLines.emplace_back(firstPoint, currentPos, camera);
			AddIntersections(straightLines.back());
		}
		break;
		case CIRCLE_ERASER:
		{
			if (objType != CIRCLE)
			{
				firstPointed = true;
				break;
			}
			if (objPos != currentObjPos)
			{
				firstPointed = true;
				firstCircleEraserPos = currentPos;
				currentObjPos = objPos;
				break;
			}
			bool eraseWholeCircle = circles[objPos].EraseSector(firstCircleEraserPos, currentPos);

			if (eraseWholeCircle || circles[objPos].CompletelyErased())
			{
				EraseObj(objType, objPos, true);
			}
			else
			{
				// update intersections (some could be in erased sectors)
				EraseObj(objType, objPos, false);
				AddIntersections(circles[objPos]);
			}
		}
			break;
		case MOVE_OBJECT:
			// add intersections again

			switch (selectedThing.objType)
			{
			case CIRCLE:
				AddIntersections(circles[selectedThing.objPos]);
				break;
			case DISTANCE:
				AddIntersections(distances[selectedThing.objPos]);
				break;
			case RAY:
				AddIntersections(rays[selectedThing.objPos]);
				break;
			case STRAIGHTLINE:
				AddIntersections(straightLines[selectedThing.objPos]);
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
	}
}

std::tuple<int, std::size_t> GeometryBoard::UpdateCurrentPoint()
{
	std::vector<long double> connectionDistances;
	std::vector<Vec2> connectionPoints;
	std::vector<int> connectionTypes;
	std::vector<std::tuple<int, std::size_t>> objPlaces;

	// other order when not firstpointed and moveobject mode
	enum ConnectionOrder
	{
		MOVING_POINT_CONNECTION,
		POINT_CONNECTION,
		INTERSECTION_CONNECTION,
		LINE_END_POINT_CONNECTION,
		LINE_CONNECTION,
		SECTOR_END_POINT_CONNECTION,
		CIRCLE_CONNECTION,
	};

	Vec2 worldMousePos = GetMousePosition2(camera);

	auto ConnectionDistancesPush = [this, &worldMousePos, &connectionDistances, &connectionPoints, &connectionTypes, &objPlaces](Vec2 &point, int connectionType, int objType, std::size_t objPos)
	{
		if (
			(connectionType == MOVING_POINT_CONNECTION && (editMode != MOVE_OBJECT || (editMode == MOVE_OBJECT && firstPointed))) ||
			((editMode == MOVE_OBJECT) && (selectedThing.objPos == objPos) && (selectedThing.objType == objType) && firstPointed))
		{
			return;
		}

		connectionDistances.push_back(GetDistance(point, worldMousePos));
		connectionPoints.push_back(point);
		connectionTypes.push_back(connectionType);
		objPlaces.push_back(std::make_tuple(objType, objPos));
	};

	int lineEndPointConnection = LINE_END_POINT_CONNECTION;

	if (!firstPointed && editMode == MOVE_OBJECT)
	{
		lineEndPointConnection = MOVING_POINT_CONNECTION;
	}

	for (std::size_t i = 0; i < distances.size(); ++i)
	{
		ConnectionDistancesPush(distances[i].pointA, lineEndPointConnection, DISTANCE, i);
		ConnectionDistancesPush(distances[i].pointB, lineEndPointConnection, DISTANCE, i);

		Vec2 intersection = GetOrthogonalLinesIntersection(worldMousePos, distances[i]);
		if (distances[i].IsPointOnLine(intersection))
		{
			ConnectionDistancesPush(intersection, LINE_CONNECTION, DISTANCE, i);
		}
	}
	for (std::size_t i = 0; i < circles.size(); ++i)
	{
		Vec2 connectionPoint = GetCircleConnection(worldMousePos, circles[i]);
		ConnectionDistancesPush(circles[i].pointOnCircle, MOVING_POINT_CONNECTION, CIRCLE, i);
		ConnectionDistancesPush(circles[i].center, MOVING_POINT_CONNECTION, CIRCLE, i);
		if (IsPointOnCircle(connectionPoint, circles[i]))
		{
			ConnectionDistancesPush(connectionPoint, CIRCLE_CONNECTION, CIRCLE, i);
		}
		
		for (auto sector : circles[i].sectors)
		{
			ConnectionDistancesPush(sector.startAnglePoint, SECTOR_END_POINT_CONNECTION, CIRCLE, i);
			ConnectionDistancesPush(sector.endAnglePoint, SECTOR_END_POINT_CONNECTION, CIRCLE, i);
		}
		
	}
	for (std::size_t i = 0; i < rays.size(); ++i)
	{
		ConnectionDistancesPush(rays[i].pointA, lineEndPointConnection, RAY, i);
		ConnectionDistancesPush(rays[i].pointB, MOVING_POINT_CONNECTION, RAY, i);

		Vec2 intersection = GetOrthogonalLinesIntersection(worldMousePos, rays[i]);
		if (rays[i].IsPointOnLine(intersection))
		{
			ConnectionDistancesPush(intersection, LINE_CONNECTION, RAY, i);
		}
	}
	for (std::size_t i = 0; i < straightLines.size(); ++i)
	{
		ConnectionDistancesPush(straightLines[i].pointA, MOVING_POINT_CONNECTION, STRAIGHTLINE, i);
		ConnectionDistancesPush(straightLines[i].pointB, MOVING_POINT_CONNECTION, STRAIGHTLINE, i);

		Vec2 intersection = GetOrthogonalLinesIntersection(worldMousePos, straightLines[i]);
		if (straightLines[i].IsPointOnLine(intersection))
		{
			ConnectionDistancesPush(intersection, LINE_CONNECTION, STRAIGHTLINE, i);
		}
	}
	for (std::size_t i = 0; i < points.size(); ++i)
	{
		ConnectionDistancesPush(points[i].GetPos(), POINT_CONNECTION, POINT, i);
	}
	for (auto &intersection : intersections.intersections)
	{
		ConnectionDistancesPush(intersection.second, INTERSECTION_CONNECTION, -1, -1);
	}

	while (!connectionDistances.empty())
	{
		auto minPos = std::min_element(connectionTypes.begin(), connectionTypes.end()) - connectionTypes.begin();
		if (connectionDistances[minPos] <= connectionDistance)
		{
			currentPoint.SetPos(connectionPoints[minPos]);
			return objPlaces[minPos];
		}
		connectionDistances.erase(connectionDistances.begin() + minPos);
		connectionPoints.erase(connectionPoints.begin() + minPos);
		connectionTypes.erase(connectionTypes.begin() + minPos);
		objPlaces.erase(objPlaces.begin() + minPos);
	}

	currentPoint.SetPos(worldMousePos);
	return std::make_tuple(-1, -1);
}

void GeometryBoard::SetEditMode()
{
	if (!IsKeyDown(KEY_LEFT_CONTROL))
	{
		return;
	}

	switch (GetKeyPressed())
	{
	case KEY_C:
		editMode = DRAW_CIRCLE;
		if (IsKeyDown(KEY_E))
		{
			editMode = CIRCLE_ERASER;
		}
		break;
	case KEY_D:
		editMode = DRAW_DISTANCE;
		break;
	case KEY_R:
		editMode = DRAW_RAY;
		break;
	case KEY_S:
		editMode = DRAW_STRAIGHT_LINE;
		if (IsKeyDown(KEY_LEFT_ALT))
		{
			app_->SetState(SAVE);
		}
		break;
	case KEY_P:
		editMode = DRAW_POINT;
		break;
	case KEY_E:
		editMode = ERASER;
		if (IsKeyDown(KEY_M))
		{
			if (IsKeyDown(KEY_L))
			{
				editMode = LENGTH_MEASUREMENT_ERASER;
			}
			else if (IsKeyDown(KEY_A))
			{
				editMode = ANGLE_MEASUREMENT_ERASER;
			}
		}
		break;
	case KEY_M:
		editMode = MOVE_OBJECT;
		if (IsKeyDown(KEY_L))
		{
			editMode = LENGTH_MEASUREMENT;
		}
		else if (IsKeyDown(KEY_A))
		{
			editMode = ANGLE_MEASUREMENT;
		}
		break;
	default:
		break;
	}
}

void GeometryBoard::InterruptDrawing()
{
	if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_LEFT_CONTROL))
	{
		firstPointed = false;
	}
}

void GeometryBoard::ModifyViewField()
{
	if (IsKeyDown(KEY_LEFT))
	{
		camera.offset.x -= movementSpeed;
	}
	else if (IsKeyDown(KEY_RIGHT))
	{
		camera.offset.x += movementSpeed;
	}

	if (IsKeyDown(KEY_UP))
	{
		camera.offset.y -= movementSpeed;
	}
	else if (IsKeyDown(KEY_DOWN))
	{
		camera.offset.y += movementSpeed;
	}

	bool resetZoom = IsKeyDown(KEY_LEFT_CONTROL) && IsKeyDown(KEY_Y) && IsKeyPressed(KEY_R);
	if (resetZoom)
	{
		camera.zoom = 1;
		camera.target = {0, 0};
		camera.offset = {0, 0};

		UpdateConnectionDistance();
	}

	float wheel = GetMouseWheelMove();
	bool zoom = wheel != 0;
	if (zoom)
	{
		// zoom to the mouse position
		// save this before setting camera.offset because it changes the camera which is needed for the target
		Vector2 mouseWorldPos = GetMousePosition2(camera).ToRaylibVec();
		camera.offset = GetMousePosition();
		camera.target = mouseWorldPos;

		camera.zoom += wheel * 0.125f;
		if (camera.zoom < minZoom)
		{
			camera.zoom = minZoom;
		}
		else if (camera.zoom > maxZoom)
		{
			camera.zoom = maxZoom;
		}

		UpdateConnectionDistance();
	}

	bool viewFieldModified = zoom || resetZoom || IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_UP) || IsKeyDown(KEY_DOWN);

	if (viewFieldModified)
	{
		UpdateAllDrawPoints();
	}
}

void GeometryBoard::UpdateAllDrawPoints()
{
	for (auto &straightLine : straightLines)
	{
		straightLine.UpdateDrawPoints(camera);
	}
	for (auto &ray : rays)
	{
		ray.UpdateDrawPoint(camera);
	}
}

bool GeometryBoard::PointLetterExists(std::string &letter)
{
	for (auto &point : points)
	{
		if (point.GetLetter() == letter)
		{
			return true;
		}
	}
	return false;
}

void GeometryBoard::AddIntersections(Circle &circle)
{
	for (auto &distance : distances)
	{
		GetLineCircleIntersections(intersections, distance, circle);
	}
	for (auto &straightLine : straightLines)
	{
		GetLineCircleIntersections(intersections, straightLine, circle);
	}
	for (auto &ray : rays)
	{
		GetLineCircleIntersections(intersections, ray, circle);
	}
	for (auto &circle_ : circles)
	{
		GetCircleCircleIntersections(intersections, circle_, circle);
	}
}

template <typename L>
void GeometryBoard::AddIntersections(L &line)
{
	for (auto &distance : distances)
	{
		GetLineLineIntersections(intersections, line, distance);
	}
	for (auto &straightLine : straightLines)
	{
		GetLineLineIntersections(intersections, line, straightLine);
	}
	for (auto &ray : rays)
	{
		GetLineLineIntersections(intersections, line, ray);
	}
	for (auto &circle : circles)
	{
		GetLineCircleIntersections(intersections, line, circle);
	}
}

void GeometryBoard::EraseObj(int objType, std::size_t objPos, bool eraseFromVector)
{
	if (objType < CIRCLE)
	{
		return;
	}

	switch (objType)
	{
	case CIRCLE:
		EraseGemObj(circles, objPos, eraseFromVector);
		break;
	case DISTANCE:
		EraseGemObj(distances, objPos, eraseFromVector);
		break;
	case RAY:
		EraseGemObj(rays, objPos, eraseFromVector);
		break;
	case STRAIGHTLINE:
		EraseGemObj(straightLines, objPos, eraseFromVector);
		break;
	case POINT:
		if (eraseFromVector)
		{
			ErasePoint(objPos);
		}
		break;
	default:
		break;
	}
}

template <typename T>
void GeometryBoard::EraseGemObj(std::vector<T> &objVec, std::size_t vecPos, bool eraseFromVector)
{
	for (auto &ID : objVec[vecPos].intersectionIDs)
	{
		intersections.intersections.erase(ID);
	}

	if (eraseFromVector)
	{
		objVec.erase(objVec.begin() + vecPos);
	}
}

void GeometryBoard::ErasePoint(std::size_t vecPos)
{
	points.erase(points.begin() + vecPos);
}

SelectedThing::SelectedThing()
{
}

template <typename L>
void SelectedThing::SetUpLineMemberObjNumber(std::vector<L> &objVector)
{
	if (originalPoint == objVector[objPos].pointA)
	{
		memberObj = 1;
	}
	else if (originalPoint == objVector[objPos].pointB)
	{
		memberObj = 2;
	}
}

SelectedThing::SelectedThing(std::size_t objPos, int objType, Vec2 &originalPoint, GeometryBoard *board_) : objPos(objPos), objType(objType), originalPoint(originalPoint), board_(board_)
{
	memberObj = 0;

	switch (objType)
	{
	case CIRCLE:
		if (originalPoint == board_->circles[objPos].center)
		{
			memberObj = 1;
		}
		else if (originalPoint == board_->circles[objPos].pointOnCircle)
		{
			memberObj = 2;
		}
		break;
	case DISTANCE:
		SetUpLineMemberObjNumber(board_->distances);
		break;
	case RAY:
		SetUpLineMemberObjNumber(board_->rays);
		break;
	case STRAIGHTLINE:
		SetUpLineMemberObjNumber(board_->straightLines);
		break;

	default:
		break;
	}
}
