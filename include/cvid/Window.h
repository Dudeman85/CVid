#pragma once
#include <vector>
#include <string>
#include <functional>
#include <unordered_map>
#define NOMINMAX
#include <windows.h>
#include <cvid/Vector.h>
#include <cvid/Types.h>

namespace cvid
{
	//Get the maximum size of a cmd window
	Vector2Int MaxWindowSize();

	//Properties of the console window which cannot be controlled by vts
	struct WindowProperties
	{
		uint16_t width;
		uint16_t height;
	};

	//Is the data a frame string or properties struct
	enum class DataType : uint8_t { String = 1, Properties = 2, Frame = 3, Ready = 4 };

	//Ascii representation of two vertically stacked pixels
	struct CharPixel
	{
		//Top pixel color
		Color fg;
		//Bottom pixel color
		Color bg;
		char character = (char)223;
	};

	//How many windows have ever been created
	static int numWindowsCreated = 0;

	//An object representing a console window process
	class Window
	{
	public:
		//Create a new console window with dimensions in console pixels
		Window(uint16_t width, uint16_t height, std::string name, bool newProcess = false);
		~Window();

		//Set a pixel on the framebuffer to the closest rgb color
		bool PutPixel(Vector2Int pos, Color color, double z = 0);
		//Set a pixel on the framebuffer to the closest rgb color
		bool PutPixel(uint16_t x, uint16_t y, Color color, double z = 0);
		//Put a character on the framebuffer, in this case y is half
		bool PutChar(Vector2Int pos, CharPixel charPixel);
		//Put a character on the framebuffer, in this case y is half
		bool PutChar(uint16_t x, uint16_t y, CharPixel charPixel);
		//Put a string of characters on the framebuffer, in this case y is half
		bool PutString(Vector2Int pos, std::string string, Color bg = { 0, 0, 0 }, Color fg = { 204, 204, 204 });
		//Put a string of characters on the framebuffer, in this case y is half
		bool PutString(uint16_t x, uint16_t y, std::string string, Color bg = { 0, 0, 0 }, Color fg = { 204, 204, 204 });
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
		//Get the input record of this console window
		std::vector<INPUT_RECORD> GetInputRecord();
		//Get the dimensions of this window. Y is in pixel coordinates
		Vector2Int GetSize();

		//Function to call when the window closes
		std::function<void(Window*)> onClose;
		//Enable depth buffering
		bool enableDepthTest = true;

		//Handle to the console input and output of this window
		HANDLE consoleOut;
		HANDLE consoleIn;
	private:
		//Create this window as a new process
		void CreateAsNewProcess(std::string name);
		//Create this window using the main application's console
		void CreateAsMain(std::string name);
		
		//Apply the properties to main console
		void ApplyPropertiesToMain(WindowProperties properties);

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
		//Is this window it's own process or the console of the parent application
		bool seperateProcess;

		//Maximum window dimensions provided by windows
		uint16_t maxWidth;
		uint16_t maxHeight;

		//PROCESS SPECIFIC
		//Pipes to send data to the window process
		std::string outPipeName;
		std::string inPipeName;
		HANDLE outPipe;
		HANDLE inPipe;
		PROCESS_INFORMATION processInfo;
	};
}