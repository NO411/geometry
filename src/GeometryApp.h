#pragma once

#include "GeometryBoard.h"
#include "raylib.h"
#include <string>

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
	void Render(Font &font);
private:
	Rectangle rectangle;
	Vector2 textPos;
	float fontSize;
	Color color;
	std::string text;

	bool Selected();

	GeometryApp &app_;
};

class GeometryApp
{
public:
	GeometryApp(int width, int height, int fps, std::string title);
	~GeometryApp() noexcept;

	void Run();
	int GetState();
	void SetState(int state);
private:
	GeometryBoard board;
	HelpButton button{*this};

	Font font;

	int appState;
	bool showHelpButton;

	bool GeometryAppShouldClose() const;
	void Tick();
	void RenderHelp();
	void Update();
};
