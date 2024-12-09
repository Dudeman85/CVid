#pragma once
#include <vector>
#include <string>
#include <functional>
#include <unordered_map>
#include <windows.h>
#include <cvid/Vector.h>
#include <cvid/Types.h>

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
	enum class DataType : uint8_t { String = 1, Properties = 2, Frame = 3 };

	//Ascii representation of two vertically stacked pixels
	struct CharPixel
	{
		//Top pixel color
		Color foregroundColor;
		//Bottom pixel color
		Color backgroundColor;
		char character = (char)223;
	};

	//How many windows have ever been created
	static int numWindowsCreated = 0;

	//An object representing a console window process
	class Window
	{
	public:
		//Create a new console window with dimensions in console pixels
		Window(uint16_t width, uint16_t height, std::string name);
		~Window();

		//Set a pixel on the framebuffer to the closest rgb color
		bool PutPixel(Vector2Int pos, Color color, float z = 0);
		//Set a pixel on the framebuffer to the closest rgb color
		bool PutPixel(uint16_t x, uint16_t y, Color color, float z = 0);
		//Put a character on the framebuffer, in this case y is half
		bool PutChar(Vector2Int pos, CharPixel charPixel);
		//Put a character on the framebuffer, in this case y is half
		bool PutChar(uint16_t x, uint16_t y, CharPixel charPixel);
		//Fills the framebuffer with a color
		bool Fill(Color color);
		//Clear the depthbuffer, setting everything to 0
		bool ClearDepthBuffer();
		//Get a modifiable reference to the depth buffer bit of a pixel
		double* GetDepthBufferBit(uint16_t x, uint16_t y);
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
		//Enable depth buffering
		bool enableDepthTest = true;

	private:
		//Bitmap of each character pixel for the window
		//Half the screen height and upside down, accessed [(height - 1 - y) / 2 * width + x] 
		CharPixel* frameBuffer;

		//Depth buffer for current z of every pixel 
		//Full screen height, accessed [y * width + x]
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