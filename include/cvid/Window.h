#pragma once
#include <vector>
#include <string>
#include <windows.h>

namespace cvid 
{
	constexpr int windowMaxWidth = 200;
	constexpr int windowMaxHeight = 200;

	using byte = unsigned char;

	class Window
	{
	private:
		//Bitmap accessed [col][row]
		std::vector<std::vector<byte>> framebuffer;
		int width, height;
		std::string name;

		std::string pipeName;
		HANDLE pipe;

		PROCESS_INFORMATION processInfo;

	public:
		//Create a new console window with dimensions in console pixels
		Window(int width, int height, std::string name);
		~Window();
	};
}