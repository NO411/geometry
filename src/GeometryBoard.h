#pragma once

#include "raylib.h"

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
	Camera2D camera;
	static const float movementSpeed;
	static const float maxZoom;

	void CheckResized();
	void InputHandler();
	void Edit();
	void SetDrawObj();
	void InterruptDrawing();
	void ModifyViewField();

	GeometryApp *app_;
};
