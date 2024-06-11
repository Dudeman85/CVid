#include <format>
#include <thread>
#include <cvid/Window.h>
#include <cvid/Rasterizer.h>
#include <cvid/Helpers.h>
#include <cvid/Renderer.h>
#include <glm/gtc/matrix_transform.hpp>

//https://gabrielgambetta.com/computer-graphics-from-scratch/

int main()
{
	cvid::Window window(60, 60, "Triangle Test");

	//Probably fix this by checking if the message was received
	//Probably need to make another pipe to send data from app to main
	//window.SendData("asdadawdgfdagsgfgd", 16, cvid::DataType::String);

	/*
	cvid::DrawTriangleWireframe(&window, { 5, 3 }, { 31, 25 }, { 55, 2 }, cvid::BrightRed);
	cvid::DrawTriangle(&window, { 5, 3 }, { 31, 25 }, { 55, 2 }, cvid::Red);

	cvid::DrawTriangleWireframe(&window, { 10, 15 }, { 25, 45 }, { 5, 45 }, cvid::BrightMagenta);
	cvid::DrawTriangle(&window, { 10, 15 }, { 25, 45 }, { 5, 45 }, cvid::Magenta);

	cvid::DrawTriangleWireframe(&window, { 50, 55 }, { 35, 45 }, { 55, 23 }, cvid::BrightYellow);
	cvid::DrawTriangle(&window, { 50, 55 }, { 35, 45 }, { 55, 23 }, cvid::Yellow);
	*/

	std::vector<cvid::Vertice> vertices{ { cvid::Vector3(10, 40, 0)}, { cvid::Vector3(-10, 12, 0)}, {cvid::Vector3(5, -2, 0)} };
	std::vector<cvid::Vector3Int> indices{ {0, 1, 2} };

	float rotation = 527;
	/*
	+p2{ x = 12.443109512329102 y = -9.4429349899291992 }	cvid::Vector2
		+ p1{ x = -0.74567317962646484 y = -41.224311828613281 }	cvid::Vector2
		+ p3{ x = -5.3217520713806152 y = 0.82398676872253418 }	cvid::Vector2
		*/

	while (true)
	{
		window.Fill(cvid::Color::Black);

		if (GetKeyState(VK_RIGHT) & 0x8000)
			rotation++;
		if (GetKeyState(VK_LEFT) & 0x8000)
			rotation--;
		if (rotation < 0)
			rotation += 360;

		std::cout << rotation << std::endl;

		cvid::DrawPoint(&window, { 19, 19 }, cvid::Color::Blue);

		glm::mat4 mvp = glm::mat4(1);

		//Position
		//mvp = glm::translate(mvp, glm::vec3(30, 30.f, 0.f));
		//X, Y, Z euler rotations
		//mvp = glm::rotate(mvp, glm::radians(rotation), glm::vec3(1.0f, 0.0f, 0.0f));
		//mvp = glm::rotate(mvp, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
		mvp = glm::rotate(mvp, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));

		//cvid::DrawVerticesWireframe(&window, vertices, indices, mvp);

		cvid::Vector2 v1(-0.74567317962646484, -41.224311828613281);
		cvid::Vector2 v2(12.443109512329102, -9.4429349899291992);
		cvid::Vector2 v3(-5.3217520713806152, 0.82398676872253418);

		//cvid::DrawPoint(&window, v2, cvid::Color::Green);

		cvid::DrawLine(&window, v1,  v2 , cvid::Color::BrightMagenta);
		cvid::DrawLine(&window,  v3 ,  v2 , cvid::Color::BrightMagenta);

		if (!window.DrawFrame())
			return 0;
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	return 0;
}