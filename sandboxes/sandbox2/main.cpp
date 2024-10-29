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

	cvid::Camera cam({ 0, 0, 0 }, 100, 64, 1);
	cam.SetRotation({ 0, 0, 0 });

	std::vector<cvid::Vertex> pyramidVertices{
		{ cvid::Vector3(0, 20, 0)},
		{ cvid::Vector3(-20, -20, 20)},
		{ cvid::Vector3(20, -20, 20)},
		{ cvid::Vector3(-20, -20, -20)},
		{ cvid::Vector3(20, -20, -20)},
	};
	std::vector<cvid::Vector3Int> pyramidIndices{
		{0, 1, 2},
		{0, 3, 1},
		{0, 3, 4},
		{0, 4, 2},
		{3, 1, 2},
		{3, 4, 2},
	};


	std::vector<cvid::Vertex> triVertices{
		{{0, 10, 0}},
		{{10, -10, 0}},
		{{-10, -10, 0}},
	};
	std::vector<cvid::Vector3Int> triIndices{
		{0, 1, 2}
	};

	std::vector<cvid::Vertex> vertices{ { cvid::Vector3(10, 40, 0)}, { cvid::Vector3(-10, 12, 0)}, {cvid::Vector3(5, -2, 0)} };
	std::vector<cvid::Vector3Int> indices{ {0, 1, 2} };
	cvid::Color col = cvid::RandomColor();


	cvid::Model cube("C:\\Users\\aleksiand\\repos\\Thesis\\resources\\Cube.obj");


	cvid::Vector3 rotation = { 0, 0, 0 };
	double pyramidRotation = 0;

	bool sd = false;

	while (true)
	{
		if (GetKeyState(VK_ESCAPE) & 0x8000)
			return 0;

		if (GetKeyState(VK_SPACE) & 0x8000)
		{
			if (!sd)
			{
				vertices[0].position = cvid::Vector3(rand() % 60 - 30, rand() % 60 - 30, 0);
				vertices[1].position = cvid::Vector3(rand() % 60 - 30, rand() % 60 - 30, 0);
				vertices[2].position = cvid::Vector3(rand() % 60 - 30, rand() % 60 - 30, 0);
				sd = true;
			}
		}
		else
			sd = false;

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
		cvid::DrawLine({ -100, 0, 0 }, { 100, 0, 0 }, cvid::Color::Red, cvid::Matrix4::Identity(), &cam, &window);
		cvid::DrawLine({ 0, -100, 0 }, { 0, 100, 0 }, cvid::Color::Green, cvid::Matrix4::Identity(), &cam, &window);
		cvid::DrawLine({ 0, 0, -100 }, { 0, 0, 100 }, cvid::Color::Blue, cvid::Matrix4::Identity(), &cam, &window);
		
		//Draw right side Triangle
		cvid::Matrix4 triangleModel = cvid::Matrix4::Identity();
		triangleModel = triangleModel.Scale({ 1.2 });
		//model = model.RotateX(cvid::Radians(rotation.x));
		triangleModel = triangleModel.RotateX(cvid::Radians(45));
		//model = model.RotateZ(cvid::Radians(rotation.z));
		triangleModel = triangleModel.Translate({ 25, 20, 0 });

		cvid::DrawVertices(triVertices, triIndices, triangleModel, &cam, &window);
		

		//Draw left side pyramid
		cvid::Matrix4 pyramidModel = cvid::Matrix4::Identity();
		pyramidModel = pyramidModel.Scale({ 10 });
		//model = model.RotateX(cvid::Radians(rotation.x));
		pyramidModel = pyramidModel.RotateY(cvid::Radians(pyramidRotation));
		//model = model.RotateZ(cvid::Radians(rotation.z));
		pyramidModel = pyramidModel.Translate({ -25, 20, 0 });

		//cvid::DrawVertices(pyramidVertices, pyramidIndices, pyramidModel, &cam, &window);
		cvid::DrawModel(&cube, pyramidModel, &cam, &window);

		cvid::DrawPoint({ 20, 20, -20 }, cvid::Color::Magenta, cvid::Matrix4::Identity(), &cam, &window);
		cvid::DrawPoint({ 20, 20, -10 }, cvid::Color::Cyan, cvid::Matrix4::Identity(), &cam, &window);

		if (!window.DrawFrame())
			return 0;
	}

	return 0;
}