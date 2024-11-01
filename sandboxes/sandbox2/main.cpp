#include <format>
#include <thread>
#include <chrono>
#include <cvid/Window.h>
#include <cvid/Rasterizer.h>
#include <cvid/Helpers.h>
#include <cvid/Renderer.h>
#include <cvid/Model.h>
#include <cvid/Matrix.h>
#include <glm/gtc/matrix_transform.hpp>
#include <tiny_obj_loader.h>

//https://gabrielgambetta.com/computer-graphics-from-scratch/
//https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-defwindowproca
//https://learn.microsoft.com/en-us/windows/win32/winmsg/using-messages-and-message-queues#examining-a-message-queue

int main()
{
	cvid::Window window(120, 80, "Projection Test");

	window.SetProperties({ 120, 90 });

	cvid::Camera cam({ 0, 0, 100 }, 100, 64, 1);
	cam.SetPerspective(90);


	cvid::Model cube("C:\\Users\\aleksiand\\repos\\Thesis\\resources\\cube.obj");


	cvid::Vector3 rotation = { 0, 0, 0 };
	double pyramidRotation = 0;

	while (true)
	{
		if (GetKeyState(VK_ESCAPE) & 0x8000)
			return 0;


		//Camera movement
		float moveSpeed = 1;
		//W forward
		if (GetKeyState(87) & 0x8000)
			cam.Translate(cam.GetForward() * moveSpeed );
		//A left
		if (GetKeyState(65) & 0x8000)
			cam.Translate(cam.GetRight() * -moveSpeed);
		//S backward
		if (GetKeyState(83) & 0x8000)
			cam.Translate(cam.GetForward() * -moveSpeed);
		//D right
		if (GetKeyState(68) & 0x8000)
			cam.Translate(cam.GetRight() * moveSpeed);
		//Q up
		if (GetKeyState(81) & 0x8000)
			cam.Translate(cam.GetUp() * moveSpeed);
		//E down
		if (GetKeyState(69) & 0x8000)
			cam.Translate(cam.GetUp() * -moveSpeed);

		//Camera rotation
		float rotationSpeed = 0.01;
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

		std::cout << cam.GetPosition().ToString() << std::endl;


		if (GetKeyState(VK_DIVIDE) & 0x8000)
			pyramidRotation++;
		if (GetKeyState(VK_MULTIPLY) & 0x8000)
			pyramidRotation--;

		window.Fill(cvid::Color::Black);
		window.ClearDepthBuffer();

		//Draw axis lines
		cvid::DrawLine({ -100, 0, 0 }, { 100, 0, 0 }, cvid::Color::Red, cvid::Matrix4::Identity(), &cam, &window); //X is red
		cvid::DrawLine({ 0, -100, 0 }, { 0, 100, 0 }, cvid::Color::Green, cvid::Matrix4::Identity(), &cam, &window); //Y is green
		cvid::DrawLine({ 0, 0, -100 }, { 0, 0, 100 }, cvid::Color::Blue, cvid::Matrix4::Identity(), &cam, &window); //Z is blue
		
		//Draw right side Triangle
		cvid::Matrix4 triangleModel = cvid::Matrix4::Identity();
		triangleModel = triangleModel.Scale({ 1.2 });
		//model = model.RotateX(cvid::Radians(rotation.x));
		triangleModel = triangleModel.RotateX(cvid::Radians(45));
		//model = model.RotateZ(cvid::Radians(rotation.z));
		triangleModel = triangleModel.Translate({ 25, 20, 0 });

		cube.faces[0].color = cvid::Color::Magenta;
		cube.faces[1].color = cvid::Color::Magenta;
		cube.faces[2].color = cvid::Color::Cyan;
		cube.faces[3].color = cvid::Color::Cyan;
		cube.faces[4].color = cvid::Color::BrightYellow;
		cube.faces[5].color = cvid::Color::BrightYellow;

		//Draw left side pyramid
		cvid::Matrix4 pyramidModel = cvid::Matrix4::Identity();
		pyramidModel = pyramidModel.Scale({ 15 });
		//model = model.RotateX(cvid::Radians(rotation.x));
		pyramidModel = pyramidModel.RotateY(cvid::Radians(pyramidRotation));
		//model = model.RotateZ(cvid::Radians(rotation.z));
		pyramidModel = pyramidModel.Translate({ -25, 20, 0 });

		//cvid::DrawVertices(pyramidVertices, pyramidIndices, pyramidModel, &cam, &window);
		cvid::DrawModel(&cube, pyramidModel, &cam, &window);

		if (!window.DrawFrame())
			return 0;

		//For some reason this stops the window from freezing
		window.SendData("\x1b[0;0H", 7, cvid::DataType::String);
	}

	return 0;
}