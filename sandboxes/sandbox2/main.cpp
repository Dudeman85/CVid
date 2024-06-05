#include <format>
#include <thread>
#include <cvid/Window.h>
#include <cvid/Rasterizer.h>
#include <cvid/Helpers.h>

int main()
{
	cvid::Window window(60, 60, "Line Test");

	//Probably fix this by checking if the message was received
	//Probably need to make another pipe to send data from app to main
	//window.SendData("asdadawdgfdagsgfgd", 16, cvid::DataType::String);

	cvid::DrawLine(&window, { 0, 0 }, {0, 59}, cvid::Color::Blue);
	cvid::DrawLine(&window, { 0, 0 }, {59, 0}, cvid::Color::Green);
	cvid::DrawLine(&window, { 0, 59 }, {59, 59 }, cvid::Color::Red);
	cvid::DrawLine(&window, { 59, 0 }, {59, 59 }, cvid::Color::Magenta);
	cvid::DrawLine(&window, { 0, 0 }, {59, 59 }, cvid::Color::Yellow);
	cvid::DrawLine(&window, { 59, 0 }, {0, 59 }, cvid::Color::BrightCyan);

	cvid::DrawTriangle(&window, { 30, 15 }, { 45, 45 }, {15, 45}, cvid::Magenta);
	cvid::DrawTriangleWireframe(&window, { 30, 15 }, { 45, 45 }, {15, 45}, cvid::RandomColor());

	window.PutPixel(30, 15, cvid::Blue);
	window.PutPixel(45, 45, cvid::Blue);
	window.PutPixel(15, 45, cvid::Blue);

	while (true)
	{
		int x = rand() % 60;
		int y = rand() % 60;

		//window.PutPixel(x, y, cvid::RandomColor());

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		window.DrawFrame();
	}

	return 0;
}