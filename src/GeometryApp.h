#pragma once

#include "GeometryBoard.h"
#include "raylib.h"
#include <string>
#include <vector>

inline const Color DARBBLUE2 = {40, 44, 52, 255}; 
inline const Color DARBBLUE3 = {33, 37, 43, 255}; 

enum AppStates
{
	GEOMETRY_BOARD,
	HELP,
};

class GeometryApp;

class HelpButton
{
public:
	HelpButton(GeometryApp &app);
	~HelpButton();
	void Update();
	void Render();
private:
	Rectangle rectangle;
	Vector2 textPos;
	float fontSize;
	Color color;
	std::string text;

	bool Selected();

	GeometryApp &app_;
};

class HelpWindow
{
public:
	HelpWindow(GeometryApp &app);
	void Render();
private:
	std::vector<std::vector<std::string>> shortcuts;
	std::string description;

	int fontSize;
	Vector2 tableStart;
	float rowSize;
	float columnSize;
	float textStart;
	float textOffsetX;

	GeometryApp &app_;
};

class GeometryApp
{
	friend HelpButton;
	friend HelpWindow;
public:
	GeometryApp(int width, int height, int fps, std::string title);
	~GeometryApp() noexcept;

	void Run();
	int GetState();
	void SetState(int state);
private:
	GeometryBoard board;
	HelpButton button{*this};
	HelpWindow helpWindow{*this};

	Font font;

	int appState;
	bool showHelpButton;

	bool GeometryAppShouldClose() const;
	void Tick();
	void RenderHelp();
	void Update();
};
