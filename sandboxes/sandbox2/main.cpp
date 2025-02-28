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

cvid::Vector2Int GetMouseDelta()
{
	return {};
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

	POINT currentPos;
	POINT lastPos;
	bool lcDown = false;

	//Camera movement
	const float rotationSpeed = 0.01;
	const float scrollSpeed = 10;

	while (true)
	{
		cvid::StartTimePoint();

		if (GetKeyState(VK_ESCAPE) & 0x8000)
			return 0;
				
		//On left click
		if (GetKeyState(VK_LBUTTON) & 0x8000)
		{
			if (!lcDown)
			{
				GetCursorPos(&currentPos);
				lastPos = currentPos;
				lcDown = true;
			}
		}
		else 
		{
			lcDown = false;
		}
		//Update cursor delta
		if (lcDown)
		{
			lastPos = currentPos;
			GetCursorPos(&currentPos);

			double dx = currentPos.x - lastPos.x;
			double dy = currentPos.y - lastPos.y;

			//Rotate model by mouse delta
			//TODO: fix to rotate around world axis
			cubeInstance.Rotate({ 0, dx * rotationSpeed, 0 });
			cubeInstance.Rotate({ dy * rotationSpeed, 0, 0 });
		}

		//Get the console input record for scroll wheel
		std::vector<INPUT_RECORD> ir = window.GetInputRecord();
		for (INPUT_RECORD& input : ir) 
		{
			if (input.EventType == MOUSE_EVENT)
			{
				if (input.Event.MouseEvent.dwEventFlags == MOUSE_WHEELED)
				{
					short scrollAmount = HIWORD(input.Event.MouseEvent.dwButtonState);
					if (scrollAmount > 0)
					{
						//Move towards when scrolling forward
						cam.Translate(cam.GetForward() * scrollSpeed);
					}
					else 
					{
						//Move away when scrolling backwards
						cam.Translate(cam.GetForward() * -scrollSpeed);
					}
				}
			}
		}

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

		if (!window.DrawFrame())
			return 0;

		//For some reason this stops the window from freezing
		window.SendData("\x1b[0;0H", 7, cvid::DataType::String);

		//std::cout << "Window responded in: " << cvid::EndTimePoint() << std::endl;
	}

	return 0;
}