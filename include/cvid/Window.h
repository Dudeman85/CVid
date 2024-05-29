#pragma once
#include <vector>
#include <string>
#include <functional>
#include <unordered_map>
#include <windows.h>
#include <cvid/math/Vector.h>

namespace cvid
{
	//Properties of the console window which cannot be controlled by vts
	struct WindowProperties
	{
		uint16_t width;
		uint16_t height;
	};

	//Is the data a frame string or properties struct
	enum DataType { String = 1, Properties = 2, Frame = 3 };
	//Color names for the Windows virtual terminal sequences, background is +10
	enum Color : uint8_t
	{
		Black = 30, Red = 31, Green = 32, Yellow = 33, Blue = 34, Magenta = 35, Cyan = 36, White = 37,
		BrightBlack = 90, BrightRed = 91, BrightGreen = 92, BrightYellow = 93, 
		BrightBlue = 94, BrightMagenta = 95, BrightCyan = 96, BrightWhite = 97
	};

	struct CharPixel
	{
		Color foregroundColor = Black;
		Color backgroundColor = Black;
		char character = (char)223;
	};

	//How many windows have ever been created
	static int numWindowsCreated = 0;

	class Window
	{
	public:
		//Create a new console window with dimensions in console pixels
		Window(uint16_t width, uint16_t height, std::string name);
		~Window();

		//Set a pixel on the framebuffer to some color
		bool PutPixel(uint16_t x, uint16_t y, Color color);
		//Set a character on the framebuffer, y is half of resolution
		bool PutChar(uint16_t x, uint16_t y, CharPixel charPixel);
		//Fills the framebuffer with a color
		bool Fill(Color color);
		//Set the properties of this window
		bool SetProperties(WindowProperties properties);
		//Draw the current framebuffer
		bool DrawFrame();
		//Send some arbitrary data to the window
		bool SendData(void* data, size_t amount, DataType type);
		//Closes the window process
		void CloseWindow();
		//Return true if the window process is still active, optionally gives back exit code
		bool IsAlive(DWORD* exitCode = nullptr);

		//Function to call when the window closes
		std::function<void(Window*)> onClose;

	private:
		//Bitmap accessed [y * width + x]
		CharPixel* framebuffer;

		//Window properties
		std::string name;
		uint16_t width;
		uint16_t height;
		bool alive = true;

		//Maximum window dimensions provided by windows
		uint16_t maxWidth;
		uint16_t maxHeight;

		//Pipe to send data to the window process
		std::string pipeName;
		HANDLE pipe;
		PROCESS_INFORMATION processInfo;
	};
}