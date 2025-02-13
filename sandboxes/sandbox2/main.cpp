#include <format>
#include <thread>
#include <chrono>
#include <cvid/Window.h>
#include <cvid/Helpers.h>
#include <cvid/Renderer.h>
#include <cvid/Model.h>
#include <cvid/Matrix.h>
#include <cvid/Math.h>
#include <cvid/Rasterizer.h>

//https://gabrielgambetta.com/computer-graphics-from-scratch/
//https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-defwindowproca
//https://learn.microsoft.com/en-us/windows/win32/winmsg/using-messages-and-message-queues#examining-a-message-queue
//https://www.scratchapixel.com/lessons/3d-basic-rendering/perspective-and-orthographic-projection-matrix/building-basic-perspective-projection-matrix.html

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	std::cout << "mouse proc";

	return 0;
}
HWND GetConsoleHwnd(void)
{
#define MY_BUFSIZE 1024
	HWND hwndFound;
	char pszNewWindowTitle[MY_BUFSIZE];
	char pszOldWindowTitle[MY_BUFSIZE];
	GetConsoleTitle(pszOldWindowTitle, MY_BUFSIZE);

	wsprintf(pszNewWindowTitle, "%d/%d",
		GetTickCount(),
		GetCurrentProcessId());

	SetConsoleTitle(pszNewWindowTitle);

	Sleep(40);

	hwndFound = FindWindow(NULL, pszNewWindowTitle);

	SetConsoleTitle(pszOldWindowTitle);

	return(hwndFound);
}

int main()
{
	cvid::Vector2Int windowSize = { 160, 90 };
	cvid::Window window(windowSize.x, windowSize.y, "CVid", false);
	window.enableDepthTest = true;

	cvid::Camera cam(cvid::Vector3(0, 0, 100), windowSize.x, windowSize.y);
	float fov = 90;
	cam.MakePerspective(fov, 1, 5000);
	cam.Rotate(cvid::Vector3(0, cvid::Radians(0), 0));

	cvid::Model cube("../../../resources/Achelous.obj");
	cvid::Texture cubeFlat("../../../resources/cubeFlat.png");

	cvid::ModelInstance cubeInstance(&cube);
	cvid::Material mat;
	mat.diffuseColor = { 25, 250, 2 };
	//mat.texture = std::make_shared<cvid::Texture>(cubeFlat);
	//cubeInstance.SetMaterial(&mat);
	cubeInstance.SetScale(20);
	cubeInstance.SetPosition({ 0, 0, 0 });
	cubeInstance.SetRotation({ 0, cvid::Radians(0), 0 });

	cvid::ModelInstance cubeInstance2(&cube);
	cubeInstance2.SetScale(10);
	cubeInstance2.SetPosition({ -60, 50, -30 });
	cubeInstance2.SetRotation({ 0, cvid::Radians(43), cvid::Radians(170) });

	HANDLE consoleIn = GetStdHandle(STD_INPUT_HANDLE);
	INPUT_RECORD inputRecord[128];
	DWORD numRead;

	HWND consoleWindow = GetConsoleHwnd();
	auto test = SetCapture(consoleWindow);
	auto err = GetLastError();

	while (true)
	{
		//Look into SetWindowsHookEx
		// https://www.reddit.com/r/cpp_questions/comments/zbhbgr/hiding_mouse_cursor_pointer_in_c_windows_console/?rdt=41214

		ReadConsoleInput(consoleIn, inputRecord, 128, &numRead);

		for (size_t i = 0; i < numRead; i++)
		{
			switch (inputRecord[i].EventType)
			{
			case MOUSE_EVENT:
				std::cout << inputRecord[i].Event.MouseEvent.dwMousePosition.X << std::endl; 
				//SetCapture(consoleWindow);
				SetCursor(NULL);
				break;
			case KEY_EVENT:
				std::cout << inputRecord[i].Event.KeyEvent.uChar.AsciiChar << std::endl;
				break;
			default:
				break;
			}
		}

		cvid::StartTimePoint();

		if (GetKeyState(VK_ESCAPE) & 0x8000)
			return 0;

		//Camera movement
		float moveSpeed = 1;
		//W forward
		if (GetKeyState(87) & 0x8000)
			cam.Translate(cam.GetForward() * moveSpeed);
		//A left
		if (GetKeyState(65) & 0x8000)
			cam.Translate(cam.GetRight() * -moveSpeed);
		//S backward
		if (GetKeyState(83) & 0x8000)
			cam.Translate(cam.GetForward() * -moveSpeed);
		//D right
		if (GetKeyState(68) & 0x8000)
			cam.Translate(cam.GetRight() * moveSpeed);
		//Q down
		if (GetKeyState(81) & 0x8000)
			cam.Translate(cam.GetUp() * -moveSpeed);
		//E up
		if (GetKeyState(69) & 0x8000)
			cam.Translate(cam.GetUp() * moveSpeed);

		//Camera rotation
		float rotationSpeed = 0.02;
		//NP 8 pitch down
		if (GetKeyState(VK_NUMPAD8) & 0x8000)
			cam.Rotate(cvid::Vector3(-1, 0, 0) * rotationSpeed);
		//NP 5 pitch down
		if (GetKeyState(VK_NUMPAD5) & 0x8000)
			cam.Rotate(cvid::Vector3(1, 0, 0) * rotationSpeed);
		//NP 4 yaw left
		if (GetKeyState(VK_NUMPAD4) & 0x8000)
			cam.Rotate(cvid::Vector3(0, 1, 0) * rotationSpeed);
		//NP 6 yaw right
		if (GetKeyState(VK_NUMPAD6) & 0x8000)
			cam.Rotate(cvid::Vector3(0, -1, 0) * rotationSpeed);
		//NP 9 roll right
		if (GetKeyState(VK_NUMPAD9) & 0x8000)
			cam.Rotate(cvid::Vector3(0, 0, -1) * rotationSpeed);
		//NP 7 roll right
		if (GetKeyState(VK_NUMPAD7) & 0x8000)
			cam.Rotate(cvid::Vector3(0, 0, 1) * rotationSpeed);

		//NP 1 fov+
		if (GetKeyState(VK_NUMPAD1) & 0x8000)
			cam.SetFOV(++fov);
		//NP 2 fov-
		if (GetKeyState(VK_NUMPAD2) & 0x8000)
			cam.SetFOV(--fov);


		if (GetKeyState('U') & 0x8000)
			cubeInstance.Rotate({ cvid::Radians(1), 0, 0 });
		if (GetKeyState('J') & 0x8000)
			cubeInstance.Rotate({ -cvid::Radians(1), 0, 0 });
		if (GetKeyState('H') & 0x8000)
			cubeInstance.Rotate({ 0, -cvid::Radians(1), 0 });
		if (GetKeyState('K') & 0x8000)
			cubeInstance.Rotate({ 0, cvid::Radians(1), 0 });
		if (GetKeyState('Y') & 0x8000)
			cubeInstance.Rotate({ 0, 0, -cvid::Radians(1) });
		if (GetKeyState('I') & 0x8000)
			cubeInstance.Rotate({ 0, 0, cvid::Radians(1) });

		window.Fill({ 0, 0, 0 });
		window.ClearDepthBuffer();

		cvid::Material mat;
		mat.diffuseColor = { 25, 250, 2 };
		//cvid::RasterizeTriangle(&window, cvid::Face{ { {5, 5, 1}, {40, 5, 10}, {40, 40, 15} }}, &mat);


		cvid::DrawModel(&cubeInstance, &cam, &window);
		//cvid::DrawModel(&cubeInstance2, &cam, &window);

		//std::cout << "Frame rendered in: " << cvid::EndTimePoint() << std::endl;

		//if (!window.DrawFrame())
			//return 0;

		//For some reason this stops the window from freezing
		window.SendData("\x1b[0;0H", 7, cvid::DataType::String);

		//std::cout << "Window responded in: " << cvid::EndTimePoint() << std::endl;
	}

	return 0;
}