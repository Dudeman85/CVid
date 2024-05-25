#pragma once
#include <vector>
#include <string>
#include <functional>
#include <windows.h>

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

	//How many windows have ever been created
	static int numWindowsCreated = 0;

	class Window
	{
	private:
		//Bitmap accessed [col][row]
		std::vector<std::vector<byte>> framebuffer;

		//Window properties
		std::string name;
		WindowProperties properties;
		short maxWidth;
		short maxHeight;

		//Callback functions
		std::function<void(Window*)> onClose;

		//Pipe to send data to the window process
		std::string pipeName;
		HANDLE pipe;

		PROCESS_INFORMATION processInfo;

		bool active = true;

	public:
		//What type of data to send 
		enum DataType { Frame, Control };

		//Create a new console window with dimensions in console pixels
		Window(int width, int height, std::string name);
		~Window();

		//Send some data to the window
		const bool SendData(const char* data, size_t amount, DataType type);
		//Set the properties of this window
		bool SetProperties(WindowProperties properties);
	};
}