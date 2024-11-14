#include <format>
#include <thread>
#include <chrono>
#include <cvid/Window.h>
#include <cvid/Helpers.h>
#include <cvid/Renderer.h>
#include <cvid/Model.h>
#include <cvid/Matrix.h>

//https://gabrielgambetta.com/computer-graphics-from-scratch/
//https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-defwindowproca
//https://learn.microsoft.com/en-us/windows/win32/winmsg/using-messages-and-message-queues#examining-a-message-queue
//https://www.scratchapixel.com/lessons/3d-basic-rendering/perspective-and-orthographic-projection-matrix/building-basic-perspective-projection-matrix.html


int main()
{
	cvid::Window window(64, 64, "Projection Test");

	//window.SetPalette(cvid::grayscale);

	cvid::Camera cam(cvid::Vector3(0, 0, 100), 80, 80);
	cam.MakePerspective();
	cam.Rotate(cvid::Vector3(0, cvid::Radians(0), 0));

	cvid::Model cube("../../../resources/cube.obj");

	/*
	cube.faces[0].color = cvid::Color::Magenta;
	cube.faces[1].color = cvid::Color::Magenta;
	cube.faces[2].color = cvid::Color::Cyan;
	cube.faces[3].color = cvid::Color::Cyan;
	cube.faces[4].color = cvid::Color::BrightYellow;
	cube.faces[5].color = cvid::Color::BrightYellow;
	*/

	cvid::ModelInstance cubeInstance(&cube);
	cubeInstance.SetScale(20);
	cubeInstance.SetPosition({ 0, 0, 0 });
	cubeInstance.SetRotation({ 0, cvid::Radians(0), 0 });

	float fov = 90;

	while (true)
	{
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


		if (GetKeyState(VK_DIVIDE) & 0x8000)
			cubeInstance.Rotate({ 0, cvid::Radians(1), 0 });
		if (GetKeyState(VK_MULTIPLY) & 0x8000)
			cubeInstance.Rotate({ 0, -cvid::Radians(1), 0 });


		window.Fill(cvid::ConsoleColor::Black);
		window.ClearDepthBuffer();

		/*
		for (int y = 0; y < window.GetDimensions().y; y++)
		{
			for (int x = 0; x < window.GetDimensions().x; x++)
			{
				window.PutPixel(x, y, { x * 4, y * 4, 0 }, 10);
			}
		}
		*/
		/*
		cvid::DrawPoint({ 40, 40, -40 }, cvid::ConsoleColor::Magenta, cvid::Matrix4::Identity(), &cam, &window);

		//Draw axis lines
		cvid::DrawLine({ -10000, 0, 0 }, { 10000, 0, 0 }, cvid::ConsoleColor::Red, cvid::Matrix4::Identity(), &cam, &window); //X is red
		cvid::DrawLine({ 0, -10000, 0 }, { 0, 10000, 0 }, cvid::ConsoleColor::Green, cvid::Matrix4::Identity(), &cam, &window); //Y is green
		cvid::DrawLine({ 0, 0, -10000 }, { 0, 0, 10000 }, cvid::ConsoleColor::Blue, cvid::Matrix4::Identity(), &cam, &window); //Z is blue
		*/


		cvid::DrawModel(&cubeInstance, &cam, &window);
		

		if (!window.DrawFrame())
			return 0;

		//For some reason this stops the window from freezing
		window.SendData("\x1b[0;0H", 7, cvid::DataType::String);

		//std::cout << cvid::EndTimePoint() << std::endl;
	}

	return 0;
}