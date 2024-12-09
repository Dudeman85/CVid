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
	//Color names for the Windows virtual terminal sequences, background is +10
	enum class ConsoleColor : uint8_t
	{
		Black = 30, Red = 31, Green = 32, Yellow = 33, Blue = 34, Magenta = 35, Cyan = 36, White = 37,
		BrightBlack = 90, BrightRed = 91, BrightGreen = 92, BrightYellow = 93,
		BrightBlue = 94, BrightMagenta = 95, BrightCyan = 96, BrightWhite = 97
	};

	//Ascii representation of two vertically stacked pixels
	struct CharPixel
	{
		//Top pixel color
		Color foregroundColor;
		//Bottom pixel color
		Color backgroundColor;
		char character = (char)223;
	};

	//CMD colors by number to corresponsing vts code, background is +10
	static std::unordered_map<uint8_t, uint8_t> ccToVTS{
		{0, 30}, {1, 34}, {2, 32}, {3, 36}, {4, 31}, {5, 35}, {6, 33}, {7, 37},
		{8, 90}, {9, 94}, {10, 92}, {11, 96}, {12, 91}, {13, 95}, {14, 93}, {15, 97} };
	//For some reason the palette numbers used by cmd are different from the ones used int VTS, this converts them
	static std::unordered_map<uint8_t, uint8_t> c2vID{
		{0, 0}, {1, 4}, {2, 2}, {3, 6}, {4, 1}, {5, 5}, {6, 3}, {7, 7},
		{8, 8}, {9, 12}, {10, 10}, {11, 14}, {12, 9}, {13, 13}, {14, 11}, {15, 15}
	};

	//Premade color palettes
	static std::unordered_map<uint8_t, Color> grayscale{
		{0, {8, 8, 8}}, {1, {24, 24, 24}}, {2, {40, 40, 40}}, {3, {56, 56, 56}}, 
		{4, {72, 72, 72}}, {5, {88, 88, 88}}, {6, {104, 104, 104}}, {7, {120, 120, 120}}, 
		{8, {136, 136, 136}}, {9, {152, 152, 152}}, {10, {168, 168, 168}}, {11, {184, 184, 184}},
		{12, {200, 200, 200}}, {13, {216, 216, 216}}, {14, {232, 232, 232}}, {15, {248, 248, 248}}
	};
	static std::unordered_map<uint8_t, Color> cmdDefault{
		{0, {12, 12, 12}}, {1, {0, 55, 218}}, {2, {19, 161, 14}}, {3, {58, 150, 221}},
		{4, {197, 15, 31}}, {5, {136, 23, 152}}, {6, {193, 156, 0}}, {7, {204, 204, 204}},
		{8, {118, 118, 118}}, {9, {59, 120, 255}}, {10, {22, 198, 12}}, {11, {97, 214, 214}},
		{12, {231, 72, 86}}, {13, {180, 0, 158}}, {14, {249, 241, 165}}, {15, {242, 242, 242}}
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
		//Set the 16 color palette to be used by the window
		void SetPalette(std::unordered_map<uint8_t, Color> palette);
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

		//A map from a console color (vts code) to the corresponding rgb value of the currently active palette
		std::unordered_map<uint8_t, Color> ccToRgb;

		//Pipes to send data to the window process
		std::string outPipeName;
		std::string inPipeName;
		HANDLE outPipe;
		HANDLE inPipe;
		PROCESS_INFORMATION processInfo;
	};
}