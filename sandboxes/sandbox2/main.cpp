#include <format>
#include <thread>
#include <cvid/Window.h>
#include <cvid/Helpers.h>

int main()
{
	cvid::Window window(60, 60, "test window");

	//Probably fix this by checking if the message was received
	//Probably need to make another pipe to send data from app to main
	//window.SendData("asdadawdgfdagsgfgd", 16, cvid::DataType::String);

	while (true)
	{
		int x = rand() % 60;
		int y = rand() % 60;

		window.PutPixel(x, y, cvid::RandomColor());

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		window.DrawFrame();
	}

	return 0;
}