#pragma once

#include "GeometryBoard.h"
#include "raylib.h"
#include <string>

enum AppStates
{
	GEOMETRY_BOARD,
	HELP,
};

class GeometryApp;

class HelpButton
{
public:
	HelpButton();
	~HelpButton();
	void Update();
	void Render(Font &font);
private:
	Vector2 center;	
	Vector2 textCenter;
	float radius;
	float ring;
	float offset;
	float fontSize;
	Color color;

	bool Selected();
};

class GeometryApp
{
public:
	GeometryApp(int width, int height, int fps, std::string title);
	~GeometryApp() noexcept;

	void Run();
private:
	GeometryBoard board;
	HelpButton button;

	Font font;

	int appState;
	bool showHelpButton;

	bool GeometryAppShouldClose() const;
	void Tick();
	void Render();
	void Update();
};
