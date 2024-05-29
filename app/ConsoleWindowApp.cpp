#include <iostream>
#include <windows.h>
#include <cvid/Window.h>
#include <cvid/Helpers.h>

int main(int argc, char* argv[])
{
	uint16_t width = 0;
	uint16_t height = 0;

	//Get the console handle
	HANDLE consoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
	HANDLE consoleIn = GetStdHandle(STD_INPUT_HANDLE);
	//Enable virtual terminal processing
	SetConsoleMode(consoleOut, ENABLE_VIRTUAL_TERMINAL_PROCESSING | ENABLE_PROCESSED_OUTPUT);
	//Disable quick edit
	DWORD mode = 0;
	GetConsoleMode(consoleIn, &mode);
	SetConsoleMode(consoleIn, mode & ~ENABLE_QUICK_EDIT_MODE);

	//Make sure we have the name of the pipe
	if (argc < 2)
	{
		cvid::LogError("CVid error in create window: No pipe given");
		system("pause");
		return -1;
	}

	//Open the pipe
	HANDLE pipe = CreateFileA(
		argv[1], //Name of the pipe
		GENERIC_READ, //Read only for this pipe
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, //Security attributes
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL //Unimportant for pipes
	);
	//Make sure the pipe connected
	if (pipe == INVALID_HANDLE_VALUE)
	{
		cvid::LogError("CVid error in create window: Failed to connect pipe, error " + std::to_string(GetLastError()));
		system("pause");
		return -2;
	}

	//Hide the cursor
	std::cout << "\x1b[?25l";

	//Update loop
	while (true)
	{
		//Read the initial setup data
		char buffer[8193];
		DWORD numBytesRead = 0;
		bool readPipeSuccess = ReadFile(
			pipe,
			buffer, //The destination for the data from the pipe
			8192 * sizeof(char), //Attempt to read this many bytes
			&numBytesRead,
			NULL //Not using overlapped IO
		);

		//Make sure the read succeeded
		if (!readPipeSuccess || numBytesRead == 0)
		{
			//If the pipe fails, kill the process
			cvid::LogWarning("CVid error in update window: Failed to read from pipe, code " + std::to_string(GetLastError()));
			return -2;
		}

		//Check the type of data received
		switch ((cvid::DataType)buffer[0])
		{
		case cvid::DataType::String:
			//Echo anything received
			buffer[numBytesRead / sizeof(char)] = '\0';
			std::cout << buffer + 1;
			break;

		case cvid::DataType::Properties:
		{
			//Get the window properties
			cvid::WindowProperties properties;
			memcpy(&properties, buffer + 1, sizeof(properties));

			width = properties.width;
			height = properties.height;

			//Resize the console to fit the frame
			SMALL_RECT consoleSize{ 0, 0, properties.width - 1 , (short)ceil((float)properties.height / 2) - 1 };
			SetConsoleWindowInfo(consoleOut, true, &consoleSize);
			if (!SetConsoleScreenBufferSize(consoleOut, { (short)properties.width, (short)ceil((float)properties.height / 2) }))
			{
				cvid::LogWarning("Error, code " + std::to_string(GetLastError()));
				//TODO: Handle error
			}
			//SetConsoleWindowInfo has to be called before and after SetConsoleScreenBufferSize othewise Windows has a fit
			if (!SetConsoleWindowInfo(consoleOut, true, &consoleSize))
			{
				cvid::LogWarning("Error, code " + std::to_string(GetLastError()));
				//TODO: Handle error
			}
			break;
		}

		case cvid::DataType::Frame:
			//Print an entire frame of data
			cvid::CharPixel* pixelData = (cvid::CharPixel*)(buffer + 1);
			std::string frameString;
			frameString.reserve((size_t)10 * width * (height / 2));

			//For every pixel in the framebuffer
			for (size_t y = 0; y < height / 2; y++)
			{
				for (size_t x = 0; x < width; x++)
				{
					cvid::CharPixel& thisPixel = pixelData[y * width + x];

					//Add the proper vts to the displayFrame
					//Format: \x1b[<fg color>;<bg color>m<char>
					frameString.append(std::format("\x1b[{};{}m{}", (uint8_t)thisPixel.foregroundColor, (uint8_t)thisPixel.backgroundColor + 10, thisPixel.character));
				}
			}

			//Move cursor to 0, 0 and print frame
			std::cout << "\x1b[0;0f" << frameString;

			break;
		}
	}
}