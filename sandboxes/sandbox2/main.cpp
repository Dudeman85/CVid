#include <format>
#include <thread>
#include <cvid/Window.h>
#include <cvid/Rasterizer.h>
#include <cvid/Helpers.h>

int main()
{
	cvid::Window window(60, 60, "Triangle Test");

	//Probably fix this by checking if the message was received
	//Probably need to make another pipe to send data from app to main
	//window.SendData("asdadawdgfdagsgfgd", 16, cvid::DataType::String);

	cvid::DrawTriangle(&window, { 10, 15 }, { 25, 45 }, {5, 45}, cvid::Magenta);
	window.PutPixel({ 10, 15 }, cvid::BrightBlue);
	window.PutPixel({ 25, 45 }, cvid::BrightBlue);
	window.PutPixel({ 5, 45 }, cvid::BrightBlue);

	cvid::DrawTriangleWireframe(&window, { 5, 3 }, { 31, 25 }, { 55, 2 }, cvid::Red);
	window.PutPixel({ 5, 3 }, cvid::BrightBlue);
	window.PutPixel({ 31, 25 }, cvid::BrightBlue);
	window.PutPixel({ 55, 2 }, cvid::BrightBlue);

	cvid::DrawTriangleWireframe(&window, { 50, 55 }, { 35, 45 }, {55, 23}, cvid::Yellow);
	window.PutPixel({ 50, 55 }, cvid::BrightBlue);
	window.PutPixel({ 35, 45 }, cvid::BrightBlue);
	window.PutPixel({ 55, 23 }, cvid::BrightBlue);

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