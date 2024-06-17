#pragma once
#include <vector>
#include <string>
#include <functional>
#include <unordered_map>
#include <windows.h>
#include <cvid/Vector.h>

namespace cvid
{
	//Properties of the console window which cannot be controlled by vts
	struct WindowProperties
	{
		uint16_t width;
		uint16_t height;
	};

	//Status message from the window process
	enum class WindowStatus : uint8_t { Ready = 1 };
	//Is the data a frame string or properties struct
	enum class DataType { String = 1, Properties = 2, Frame = 3 };
	//Color names for the Windows virtual terminal sequences, background is +10
	enum class Color : uint8_t
	{
		Black = 30, Red = 31, Green = 32, Yellow = 33, Blue = 34, Magenta = 35, Cyan = 36, White = 37,
		BrightBlack = 90, BrightRed = 91, BrightGreen = 92, BrightYellow = 93,
		BrightBlue = 94, BrightMagenta = 95, BrightCyan = 96, BrightWhite = 97
	};

	//Ascii representation of two vertically stacked pixels
	struct CharPixel
	{
		Color foregroundColor = Color::Black;
		Color backgroundColor = Color::Black;
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

		//Set a pixel on the framebuffer to some color, returns true on success
		bool PutPixel(Vector2Int pos, Color color);
		//Set a pixel on the framebuffer to some color
		bool PutPixel(uint16_t x, uint16_t y, Color color);
		//Put a character on the framebuffer, in this case y is half
		bool PutChar(Vector2Int pos, CharPixel charPixel);
		//Put a character on the framebuffer, in this case y is half
		bool PutChar(uint16_t x, uint16_t y, CharPixel charPixel);
		//Fills the framebuffer with a color
		bool Fill(Color color);
		//Draw the current framebuffer
		bool DrawFrame();
		//Send some arbitrary data to the window
		bool SendData(const void* data, size_t amount, DataType type, bool block = true);
		//Set the properties of this window, clears the framebuffer
		bool SetProperties(WindowProperties properties);
		//Closes the window process
		void CloseWindow();
		//Return true if the window process is still active, optionally gives back exit code
		bool IsAlive(DWORD* exitCode = nullptr);
		//Get the dimensions of this window. Y is in pixel coordinates
		Vector2Int GetDimensions();

		//Function to call when the window closes
		std::function<void(Window*)> onClose;
		//Should adaptive palette swapping be used
		bool useAdaptivePalette = false;

	private:
		//Bitmap of each character pixel for the window
		//Half the screen height, accessed [y * width + x] 
		CharPixel* framebuffer;

		//Depth buffer for current z of every pixel 
		//Half the screen height, accessed [y * width + x]
		//TODO implementation
		double* depthBuffer;

		//Window properties
		std::string name;
		uint16_t width;
		uint16_t height;
		bool alive = true;

		//Maximum window dimensions provided by windows
		uint16_t maxWidth;
		uint16_t maxHeight;

		//Pipes to send data to the window process
		std::string outPipeName;
		std::string inPipeName;
		HANDLE outPipe;
		HANDLE inPipe;
		PROCESS_INFORMATION processInfo;
	};
}