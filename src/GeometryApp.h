#pragma once

#include "GeometryBoard.h"
#include "raylib.h"
#include <string>
#include <vector>

enum AppStates
{
	GEOMETRY_BOARD,
	HELP,
	SAVE,
};

class Window;
class HelpWindow;

class ScrollBar
{
	friend Window;

public:
	ScrollBar();
	ScrollBar(Rectangle boxRect, Window *window_);

	void Update();
	void Render();
	void UpdateOnResize();
	bool Enabled();
	
	Rectangle box;
private:
	bool Selected();
	bool BoxSelected();

	void UpdateDragBoxHeight();

	Rectangle dragBox;
	Color dragBoxColor;

	bool move = false;
	Vector2 movePos;
	float moveStartY;

	Window *window_;
};

class Window
{
public:
	Window();

	ScrollBar scrollBar;
	Camera2D camera;
	float textRenderHeight;

	GeometryApp *app_;
};

class HelpWindow : public Window
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

	int fontSize;
	Vector2 tableStart;
	float rowSize;
	float columnSize;
	float textStart;
	float textOffsetX;
};

class Button
{
public:
	Button();

	Color color;
	Vec2 pos;
	bool pressed;

	GeometryApp *app_;
};

class CloseButton : public Button
{
public:
	CloseButton();
	CloseButton(GeometryApp *app);

	void Render();
	void Update();
	void Init();
private:
	bool Selected();

	Vec2 crossPointOffset;
	Vec2 crossPointOffset2;
	float radius;

	static const Color RED1;
	static const Color RED2;
};

class SaveWindow : public Window
{
public:
	SaveWindow(GeometryApp *app);

	void Render();
	void Update();
	void Init();
private:
	CloseButton closeButton;
};

class HelpButton : public Button
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
	float fontSize;
	std::string text;
	bool showHelpButton;
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
	SaveWindow saveWindow{this};

	Font font;

	int appState;

	bool GeometryAppShouldClose() const;
	void Tick();
	void RenderHelp();
	void Update();
	void ClearBoard();
};
