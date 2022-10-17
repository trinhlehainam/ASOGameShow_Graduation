#include <windows.h>

#include "Systems/Application.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	auto& app = Application::Instance();
	app.Init();
	app.Run();
	app.Exit();

	return 0;
}