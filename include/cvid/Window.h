#pragma once
#include <vector>
#include <string>
#include <functional>
#include <windows.h>
#include <cvid/math/Vector.h>

namespace cvid
{
	constexpr int windowMaxWidth = 200;
	constexpr int windowMaxHeight = 200;

	using byte = unsigned char;

	//Properties of the console window which cannot be controlled by vts
	struct WindowProperties
	{
		uint16_t width;
		uint16_t height;
	};

	//Is the data a frame string or properties struct
	enum DataType { Frame, Properties };
	//Color names for the Windows console
	enum Color
	{
		Black = 0, Blue = 1, Green = 2, Aqua = 3, Red = 4, Purple = 5, Yellow = 6,
		White = 7, Gray = 8, LightBlue = 9, LightGreen = 10, LightAqua = 11,
		LightRed = 12, LightPurple = 13, LightYellow = 14, BrightWhite = 16
	};

	//How many windows have ever been created
	static int numWindowsCreated = 0;

	class Window
	{
	public:

		//Create a new console window with dimensions in console pixels
		Window(int width, int height, std::string name);
		~Window();

		//Set a pixel on the framebuffer to some color, returns true on success
		bool PutPixel(int x, int y, Color color);
		//Set the properties of this window, returns true on success
		bool SetProperties(WindowProperties properties);
		//Draw the current framebuffer
		bool DrawFrame();
		//Send some arbitrary data to the window, returns true on success
		bool SendData(const char* data, size_t amount, DataType type);

		//Function to call when the window closes
		std::function<void(Window*)> onClose;

	private:
		//Bitmap accessed [col][row]
		std::vector<std::vector<Color>> framebuffer;

		//Window properties
		std::string name;
		WindowProperties properties;
		bool active = true;

		short maxWidth;
		short maxHeight;

		//Pipe to send data to the window process
		std::string pipeName;
		HANDLE pipe;
		PROCESS_INFORMATION processInfo;
	};
}