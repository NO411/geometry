#include "GeometryApp.h"
#include "Settings.h"

int main()
{
	GeometryApp app(settings::screenWidth, settings::screenHeight, settings::fps, "Geometry");
	app.Run();

	return 0;
}
