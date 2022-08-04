#pragma once

#include "GeometryBoard.h"
#include "raylib.h"
#include <string>
#include <vector>

inline const Color DARBBLUE2 = {50, 50, 60, 255};
inline const Color DARBBLUE3 = {30, 30, 40, 255};

enum AppStates
{
	GEOMETRY_BOARD,
	HELP,
};

class HelpButton
{
public:
	HelpButton(GeometryApp *app);
	~HelpButton();
	void Update();
	void Render();
	bool Selected();

private:
	Rectangle rectangle;
	Vector2 textPos;
	float fontSize;
	Color color;
	std::string text;

	bool pressed;

	GeometryApp *app_;
};

class HelpWindow
{
public:
	HelpWindow(GeometryApp *app);
	void CalculateKeyHighlightings();
	void Render();

private:
	std::vector<std::vector<std::string>> shortcuts;
	std::vector<Rectangle> keyHighlightings;
	std::string description;

	int fontSize;
	Vector2 tableStart;
	float rowSize;
	float columnSize;
	float textStart;
	float textOffsetX;

	GeometryApp *app_;
};

class GeometryApp
{
	friend HelpButton;
	friend HelpWindow;
	friend GeometryBoard;

public:
	GeometryApp(int width, int height, int fps, std::string title);
	~GeometryApp() noexcept;

	void Run();
	int GetState();
	void SetState(int state);

private:
	GeometryBoard board{this};
	HelpButton button{this};
	HelpWindow helpWindow{this};

	Font font;

	int appState;
	bool showHelpButton;

	bool GeometryAppShouldClose() const;
	void Tick();
	void RenderHelp();
	void Update();
};
