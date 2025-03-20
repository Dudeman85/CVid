#include <cvid/Window.h>
#include <cvid/Renderer.h>
#include <cvid/Model.h>
#include <cvid/Rasterizer.h>
#include <cvid/Camera.h>
#include "stdio.h"
#include <iostream>
#include <Windows.h>

int main()
{
	/*
	//Make a console window with width, height, and name
	cvid::Window window(160, 90, "CVid");
	window.enableDepthTest = false;

	//Make the camera with {x, y, z}, width, height
	cvid::Camera cam({ 0, 0, 100 }, 64, 64);
	//Set it as perspective with fov, near, and far
	cam.MakePerspective(90, 1, 100);
	*/

	char buff[200];
	GetConsoleOriginalTitle(buff, 200);

	while (true) 
	{
		std::cout << (GetKeyState(VK_NUMPAD8) & 0x8000) << std::endl;
	}

	system("pause");

	return 0;
}