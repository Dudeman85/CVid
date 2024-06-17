#include <format>
#include <thread>
#include <chrono>
#include <cvid/Window.h>
#include <cvid/Rasterizer.h>
#include <cvid/Helpers.h>
#include <cvid/Renderer.h>
#include <glm/gtc/matrix_transform.hpp>
#include <tiny_obj_loader.h>
#include <cvid/Matrix.h>

//https://gabrielgambetta.com/computer-graphics-from-scratch/
//https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-defwindowproca
//https://learn.microsoft.com/en-us/windows/win32/winmsg/using-messages-and-message-queues#examining-a-message-queue

int main()
{
	HWND cwind = GetConsoleWindow();

	cvid::Window window(60, 60, "Triangle Test");

	window.SetProperties({ 100, 64 });

	std::vector<cvid::Vertice> pyramidVertices{
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

	std::vector<cvid::Vertice> vertices{ { cvid::Vector3(10, 40, 0)}, { cvid::Vector3(-10, 12, 0)}, {cvid::Vector3(5, -2, 0)} };
	std::vector<cvid::Vector3Int> indices{ {0, 1, 2} };
	cvid::Color col = cvid::RandomColor();

	cvid::Vector3 rotation = 0;

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

		if (GetKeyState(VK_RIGHT) & 0x8000)
			rotation += 1;
		if (GetKeyState(VK_LEFT) & 0x8000)
			rotation -= 1;

		window.Fill(cvid::Color::Black);
		window.ClearDepthBuffer();

		//GLM matrix
		glm::mat4 glmModel(1);
		glmModel = glm::rotate(glmModel, (float)glm::radians(rotation.x), { 1.0, 0, 0 });
		glmModel = glm::rotate(glmModel, (float)glm::radians(rotation.y), { 0, 1.0, 0 });
		glmModel = glm::rotate(glmModel, (float)glm::radians(rotation.z), { 0, 0, 1.0 });
		glmModel = glm::translate(glmModel, { 0, 0, 20 });

		cvid::DrawVerticesWireframe(&window, pyramidVertices, pyramidIndices, glmModel);

		//My matrix
		cvid::Matrix4 model = cvid::Matrix4::Identity();
		model = model.RotateX(cvid::Radians(rotation.x));
		model = model.RotateY(cvid::Radians(rotation.y));
		model = model.RotateZ(cvid::Radians(rotation.z));
		model = model.Translate({0, 0, 20});

		//cvid::DrawVertices(&window, vertices, indices, mvp);
		cvid::DrawVerticesWireframe(&window, pyramidVertices, pyramidIndices, model);

		if (!window.DrawFrame())
			return 0;
	}

	return 0;
}