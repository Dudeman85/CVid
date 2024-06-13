#include <format>
#include <thread>
#include <chrono>
#include <cvid/Window.h>
#include <cvid/Rasterizer.h>
#include <cvid/Helpers.h>
#include <cvid/Renderer.h>
#include <glm/gtc/matrix_transform.hpp>

//https://gabrielgambetta.com/computer-graphics-from-scratch/
//https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-defwindowproca
//https://learn.microsoft.com/en-us/windows/win32/winmsg/using-messages-and-message-queues#examining-a-message-queue

int main()
{
	HWND cwind = GetConsoleWindow();

	cvid::Window window(60, 60, "Triangle Test");

	//Probably fix this by checking if the message was received
	//Probably need to make another pipe to send data from app to main
	//window.SetProperties({100, 61});

	std::vector<cvid::Vertice> vertices{ { cvid::Vector3(10, 40, 0)}, { cvid::Vector3(-10, 12, 0)}, {cvid::Vector3(5, -2, 0)} };
	std::vector<cvid::Vector3Int> indices{ {0, 1, 2} };
	cvid::Color col = cvid::RandomColor();

	float rotation = 0;

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
			rotation++;
		if (GetKeyState(VK_LEFT) & 0x8000)
			rotation--;
		if (rotation < 0)
			rotation += 360;

		window.Fill(cvid::Color::Black);

		glm::mat4 mvp = glm::mat4(1);
		//Position
		//mvp = glm::translate(mvp, glm::vec3(30, 30.f, 0.f));
		//X, Y, Z euler rotations
		//mvp = glm::rotate(mvp, glm::radians(rotation), glm::vec3(1.0f, 0.0f, 0.0f));
		//mvp = glm::rotate(mvp, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
		mvp = glm::rotate(mvp, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));

		cvid::DrawVertices(&window, vertices, indices, mvp);

		if (!window.DrawFrame())
			return 0;
	}

	return 0;
}