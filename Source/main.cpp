#include <windows.h>

#include "Systems/Application.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	auto& sceneMng = Application::Instance();
	sceneMng.Init();
	sceneMng.Run();
	sceneMng.Exit();

	return 0;
}