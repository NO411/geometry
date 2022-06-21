#pragma once

#include "raylib.h"
#include <string>

enum AppStates
{
	GEOMETRY_BOARD,
	HELP,
};


class GeometryApp
{
public:
	GeometryApp(int width, int height, int fps, std::string title);
	~GeometryApp() noexcept;

	void Run();
private:
	Camera2D camera;
	Font font;

	bool GeometryAppShouldClose() const;
	void Tick();
	void Draw();
	void Update();
};
