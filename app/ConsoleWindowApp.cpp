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
		return -1;
	}

	//Pipes
	std::string genericPipeName = std::string(argv[1]);
	//Open the outbound pipe
	HANDLE outPipe = CreateFileA(
		(genericPipeName + "in").c_str(), //Inbound to server
		GENERIC_WRITE, //Write only for this pipe
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, //Security attributes
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL //Unimportant for pipes
	);
	//Make sure the pipe connected
	if (outPipe == INVALID_HANDLE_VALUE)
	{
		cvid::LogError("CVid error in create window: Failed to connect pipe, error " + std::to_string(GetLastError()));
		return -2;
	}

	//Open the inbound pipe
	HANDLE inPipe = CreateFileA(
		(genericPipeName + "out").c_str(), //Outbound from server
		GENERIC_READ, //Read only for this pipe
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, //Security attributes
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL //Unimportant for pipes
	);
	//Make sure the pipe connected
	if (inPipe == INVALID_HANDLE_VALUE)
	{
		cvid::LogError("CVid error in create window: Failed to connect pipe, error " + std::to_string(GetLastError()));
		return -2;
	}

	//Send a ready status
	char cdata = (char)cvid::WindowStatus::Ready;
	bool sendSuccess = WriteFile(
		outPipe,
		&cdata,
		1 * sizeof(char), //How many bytes to send
		NULL, NULL //Irrelevant
	);

	//Make sure the data was sent
	if (!sendSuccess)
	{
		cvid::LogWarning("CVid warning in Window: Failed to send status message " + std::to_string(GetLastError()));
		return -3;
	}

	//Hide the cursor
	std::cout << "\x1b[?25l";

	size_t bufferSize = (sizeof(cvid::WindowProperties) + 1) * sizeof(char);
	char* buffer = new char[bufferSize];

	//Update loop
	while (true)
	{
		//Read the data from the pipe
		DWORD numBytesRead = 0;
		bool readPipeSuccess = ReadFile(
			inPipe,
			buffer, //The destination for the data from the pipe
			bufferSize * sizeof(char), //Attempt to read this many bytes
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

			//Resize the buffer
			bufferSize = (size_t)width * (height / 2) * sizeof(cvid::CharPixel);
			delete[] buffer;
			buffer = new char[bufferSize];
			break;
		}

		case cvid::DataType::Frame:
			//Print an entire frame of data
			cvid::CharPixel* pixelData = (cvid::CharPixel*)(buffer + 1);
			std::string frameString;
			frameString.reserve((size_t)29 * width * (height / 2));

			//Move cursor to 0, 0 and set colors to black
			std::cout << "\x1b[0;0f\x1b[38;2;0;0;0m\x1b[48;2;0;0;0m";

			cvid::Color currentBg{ 0, 0, 0 };
			cvid::Color currentFg{ 0, 0, 0 };
			//For every pixel in the framebuffer
			for (size_t y = 0; y < height / 2; y++)
			{
				for (size_t x = 0; x < width; x++)
				{
					cvid::CharPixel& thisPixel = pixelData[y * width + x];

					if (thisPixel.fg.r != currentFg.r || thisPixel.fg.g != currentFg.g || thisPixel.fg.b != currentFg.b)
					{
						//Add the proper vts to the displayFrame
						//Format: \x1b38;2;<r>;<g>;<b>;m
						frameString.append(std::format("\x1b[38;2;{};{};{}m", thisPixel.fg.r, thisPixel.fg.g, thisPixel.fg.b));
						//std::cout << std::format("\x1b[38;2;{};{};{}m", thisPixel.fg.r, thisPixel.fg.g, thisPixel.fg.b);
						currentFg = thisPixel.fg;
					}
					if (thisPixel.bg.r != currentBg.r || thisPixel.bg.g != currentBg.g || thisPixel.bg.b != currentBg.b)
					{
						frameString.append(std::format("\x1b[48;2;{};{};{}m", thisPixel.bg.r, thisPixel.bg.g, thisPixel.bg.b));
						//std::cout << std::format("\x1b[48;2;{};{};{}m", thisPixel.bg.r, thisPixel.bg.g, thisPixel.bg.b);
						currentBg = thisPixel.bg;
					}
					frameString += thisPixel.character;
				}
			}

			//Print frame
			std::cout << frameString ;

			break;
		}

		//Send a ready status
		char cdata = (char)cvid::WindowStatus::Ready;
		bool sendSuccess = WriteFile(
			outPipe,
			&cdata,
			1 * sizeof(char), //How many bytes to send
			NULL, NULL //Irrelevant
		);

		//Make sure the data was sent
		if (!sendSuccess)
		{
			cvid::LogWarning("CVid warning in Window: Failed to send status message " + std::to_string(GetLastError()));
			return -3;
		}
	}
}