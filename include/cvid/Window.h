#pragma once
#include <vector>
#include <string>
#include <functional>
#include <unordered_map>
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
	enum DataType { String = 1, Properties = 2 };
	//Color names for the Windows virtual terminal sequences, background is +10
	enum Color
	{
		Black = 30, Red = 31, Green = 32, Yellow = 33, Blue = 34, Magenta = 35, Cyan = 36, White = 37,
		BrightBlack = 90, BrightRed = 91, BrightGreen = 92, BrightYellow = 93, 
		BrightBlue = 94, BrightMagenta = 95, BrightCyan = 96, BrightWhite = 97
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