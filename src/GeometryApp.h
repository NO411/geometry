#pragma once

#include "GeometryBoard.h"
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
	GeometryBoard board;

	Font font;
	int appState = GEOMETRY_BOARD;

	bool GeometryAppShouldClose() const;
	void Tick();
	void Draw();
	void Update();
};
