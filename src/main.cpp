#include "GeometryApp.h"
#include "Settings.h"
#include "raylib.h"

int main()
{
	GeometryApp app(settings::screenWidth, settings::screenHeight, settings::fps, "Geometry");
	app.Run();

	return 0;
}
