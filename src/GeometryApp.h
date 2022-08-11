#pragma once

#include "GeometryBoard.h"
#include "raylib.h"
#include <string>
#include <vector>

enum AppStates
{
	GEOMETRY_BOARD,
	HELP,
};

class HelpWindow;

class ScrollBar
{
	friend HelpWindow;
public:
	ScrollBar();
	ScrollBar(Rectangle boxRect, HelpWindow *helpWindow_);

	void Update();
	void Render();
private:
	bool Selected();
	bool BoxSelected();

	void UpdateDragBoxHeight();

	Rectangle box;
	Rectangle dragBox;
	Color dragBoxColor;

	bool move = false;
	Vector2 movePos;
	float moveStartY;

	HelpWindow *helpWindow_;
};

class HelpWindow
{
	friend ScrollBar;
public:
	HelpWindow(GeometryApp *app);
	void CalculateKeyHighlightings();
	void Render();
	void Update();
	void Init();

private:
	std::vector<std::vector<std::string>> shortcuts;
	std::vector<Rectangle> keyHighlightings;
	std::string description;

	Camera2D camera;
	ScrollBar scrollBar;

	int fontSize;
	Vector2 tableStart;
	float rowSize;
	float columnSize;
	float textStart;
	float textOffsetX;

	float textRenderHeight;

	GeometryApp *app_;
};

class HelpButton
{
	friend HelpWindow;
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

	static const Color DARBBLUE1;
	static const Color DARBBLUE2;
	static const Color DARBBLUE3;

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
	void ClearBoard();
};
