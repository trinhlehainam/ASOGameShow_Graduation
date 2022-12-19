#include <windows.h>

#include "Systems/Application.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	Application::Initialize();
	Application::Execute();
	Application::Terminate();

	return 0;
}