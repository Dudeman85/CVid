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
	cvid::Window window(120, 80, "MVP Test");

	window.SetProperties({ 120, 90 });

	cvid::Camera cam({ 0, 0, 100 }, 100, 64, 1);
	cam.SetPerspective(90);


	cvid::Model cube("C:\\Users\\aleksiand\\repos\\Thesis\\resources\\suzanne.obj");


	cvid::Vector3 rotation = { 0, 0, 0 };
	double pyramidRotation = 0;

	while (true)
	{
		if (GetKeyState(VK_ESCAPE) & 0x8000)
			return 0;

		//Camera rotation
		if (GetKeyState(VK_RIGHT) & 0x8000)
			rotation.y += 1;
		if (GetKeyState(VK_LEFT) & 0x8000)
			rotation.y -= 1;
		if (GetKeyState(VK_UP) & 0x8000)
			rotation.x += 1;
		if (GetKeyState(VK_DOWN) & 0x8000)
			rotation.x -= 1;

		//camera movement
		float moveSpeed = 1;
		if (GetKeyState(87) & 0x8000)
			cam.Translate({ 0, 0, -moveSpeed });
		if (GetKeyState(65) & 0x8000)
			cam.Translate({ -moveSpeed, 0, 0 });
		if (GetKeyState(83) & 0x8000)
			cam.Translate({ 0, 0, moveSpeed });
		if (GetKeyState(68) & 0x8000)
			cam.Translate({ moveSpeed, 0, 0 });


		if (GetKeyState(VK_DIVIDE) & 0x8000)
			pyramidRotation++;
		if (GetKeyState(VK_MULTIPLY) & 0x8000)
			pyramidRotation--;

		window.Fill(cvid::Color::Black);
		window.ClearDepthBuffer();

		cam.SetRotation(rotation * (cvid::PI / 180));

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


		//Draw left side pyramid
		cvid::Matrix4 pyramidModel = cvid::Matrix4::Identity();
		pyramidModel = pyramidModel.Scale({ 15 });
		//model = model.RotateX(cvid::Radians(rotation.x));
		pyramidModel = pyramidModel.RotateY(cvid::Radians(pyramidRotation));
		//model = model.RotateZ(cvid::Radians(rotation.z));
		pyramidModel = pyramidModel.Translate({ -25, 20, 0 });

		//cvid::DrawVertices(pyramidVertices, pyramidIndices, pyramidModel, &cam, &window);
		cvid::DrawModel(&cube, pyramidModel, &cam, &window);

		//Draw the camera's facing vector from origin
		cvid::DrawLine(cvid::Vector3(0, 0, 0), cam.GetFacing() * 100, cvid::Color::Magenta, cvid::Matrix4::Identity(), &cam, &window);

		if (!window.DrawFrame())
			return 0;

		//For some reason this stops the window from freezing
		window.SendData("\x1b[0;0H", 7, cvid::DataType::String);
	}

	return 0;
}