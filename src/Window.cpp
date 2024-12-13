#include <cvid/Window.h>
#include <cvid/Helpers.h>
#include <format>

namespace cvid
{
	Vector2Int MaxWindowSize()
	{
		HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
		return Vector2Int(GetLargestConsoleWindowSize(console).X, GetLargestConsoleWindowSize(console).Y * 2);
	};

	//Create a new console window
	Window::Window(uint16_t width, uint16_t height, std::string name)
	{
		//Round height to upper multiple of 2
		height += height % 2;
		//Setup some basic variables
		this->width = width;
		this->height = height;
		this->name = name;
		HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
		maxWidth = GetLargestConsoleWindowSize(console).X;
		maxHeight = GetLargestConsoleWindowSize(console).Y * 2;

		//Create the frame and depth buffers
		frameBuffer = new CharPixel[(size_t)width * height / 2];
		depthBuffer = new double[(size_t)width * height];
		ClearDepthBuffer();

		//Create the outbound pipe to the new console process
		unsigned int pid = GetCurrentProcessId();
		std::string genericPipeName = std::format("\\\\.\\pipe\\process{}window{}", pid, numWindowsCreated);
		outPipeName = genericPipeName + "out";
		outPipe = CreateNamedPipeA(
			outPipeName.c_str(),
			PIPE_ACCESS_OUTBOUND,
			PIPE_TYPE_MESSAGE | PIPE_READMODE_BYTE | PIPE_NOWAIT,
			1, //Max instances
			16386, //Output buffer size 
			0, //Input buffer size
			1000, //Time out in ms
			NULL //Security attributes
		);
		//Make sure the pipe creation worked
		if (outPipe == NULL || outPipe == INVALID_HANDLE_VALUE)
		{
			LogError("CVid error in Window: Failed to create pipe, code " + std::to_string(GetLastError()));
			throw std::runtime_error("Failed to create pipe");
			return;
		}

		//Create the inbound pipe from the new console process
		inPipeName = genericPipeName + "in";
		inPipe = CreateNamedPipeA(
			inPipeName.c_str(),
			PIPE_ACCESS_INBOUND,
			PIPE_TYPE_MESSAGE | PIPE_READMODE_BYTE | PIPE_WAIT,
			1, //Max instances
			0, //Output buffer size 
			8, //Input buffer size
			50, //Time out in ms
			NULL //Security attributes
		);
		//Make sure the pipe creation worked
		if (inPipe == NULL || inPipe == INVALID_HANDLE_VALUE)
		{
			LogError("CVid error in Window: Failed to create pipe, code " + std::to_string(GetLastError()));
			throw std::runtime_error("Failed to create pipe");
			return;
		}

		//Create an instance of the ConsoleWindowApp
		//Setup startup info
		STARTUPINFO startupInfo;
		ZeroMemory(&startupInfo, sizeof(startupInfo));
		ZeroMemory(&processInfo, sizeof(processInfo));
		startupInfo.cb = sizeof(startupInfo);
		startupInfo.lpTitle = name.data();

		//Create the process
		std::string cmdline("ConsoleWindowApp.exe ");
		cmdline.append(genericPipeName);
		bool createProcessSuccess = CreateProcessA(
			NULL, //App name, default to cmd
			cmdline.data(), //Comand line, send the pipe name
			NULL, NULL, //Security attributes
			false, //Inherit handles
			NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE | CREATE_NEW_PROCESS_GROUP, //Creation flags
			NULL, NULL, //Irrelevant
			&startupInfo,
			&processInfo
		);
		//Make sure the process creation worked
		if (!createProcessSuccess)
		{
			LogError("CVid error in Window: Failed to create window process, code " + std::to_string(GetLastError()));
			throw std::runtime_error("Failed to create process");
			return;
		}

		//Connect the pipes
		bool connectPipeSuccess = ConnectNamedPipe(inPipe, NULL);
		if (!connectPipeSuccess)
		{
			//Some other error
			LogError("CVid error in Window: Failed to connect pipe, code " + std::to_string(GetLastError()));
			CloseHandle(inPipe);
			throw std::runtime_error("Failed to connect pipe");
			return;
		}

		connectPipeSuccess = ConnectNamedPipe(outPipe, NULL);
		//Make sure the pipe connected successfully
		while (!connectPipeSuccess)
		{
			//Wait for process to connect
			if (GetLastError() == ERROR_PIPE_LISTENING)
			{
				connectPipeSuccess = ConnectNamedPipe(outPipe, NULL);
				continue;
			}

			if (GetLastError() == ERROR_PIPE_CONNECTED)
				break;

			//Some other error
			LogError("CVid error in Window: Failed to connect pipe, code " + std::to_string(GetLastError()));
			CloseHandle(outPipe);
			throw std::runtime_error("Failed to connect pipe");
			return;
		}

		//Set the properties of the Window
		WindowProperties properties{ width, height };
		SetProperties(properties);

		numWindowsCreated++;
	}

	Window::~Window()
	{
		CloseWindow();
	}

	//Set a pixel on the framebuffer to some color, returns true on success
	bool Window::PutPixel(Vector2Int pos, Color color, double z)
	{
		return PutPixel(pos.x, pos.y, color, z);
	}
	//Set a pixel on the framebuffer to some color, returns true on success
	bool Window::PutPixel(uint16_t x, uint16_t y, Color color, double z)
	{
		//Make sure the pixel is in bounds
		if (x >= width || y >= height || z < 0)
			return false;

		//Make sure there is not already a closer pixel
		if (enableDepthTest)
		{
			//Basically smaller z means further away, +0.01 prevents edge fighting
			if (z < depthBuffer[y * width + x] + 0.01)
				return false;
			depthBuffer[y * width + x] = z;
		}

		//Pixels are formatted two above each other in one character
		//We will always print 223 where foreground is the top and background is the bottom.
		CharPixel& thisPixel = frameBuffer[((height - 1 - y) / 2) * width + x];

		//Set the pixel character
		thisPixel.character = (char)223;
		//Top or bottom pixel
		if (y % 2 == 0)
			thisPixel.backgroundColor = color;
		else
			thisPixel.foregroundColor = color;

		return true;
	}

	//Set a character on the framebuffer, y is half of resolution
	bool Window::PutChar(Vector2Int pos, CharPixel charPixel)
	{
		return PutChar(pos.x, pos.y, charPixel);
	}
	//Set a character on the framebuffer, y is half of resolution
	bool Window::PutChar(uint16_t x, uint16_t y, CharPixel charPixel)
	{
		//Make sure the pixel is in bounds
		if (x >= width || y >= height / 2)
		{
			LogWarning("CVid warning in PutPixel: Position out of range");
			return false;
		}

		frameBuffer[y * width + x] = charPixel;

		return true;
	}

	//Put a string of characters on the framebuffer, in this case y is half
	bool Window::PutString(Vector2Int pos, std::string string, Color bg, Color fg)
	{
		return PutString(pos.x, pos.y, string, bg, fg);
	}
	//Put a string of characters on the framebuffer, in this case y is half
	bool Window::PutString(uint16_t x, uint16_t y, std::string string, Color bg, Color fg)
	{
		//For each character
		for (size_t i = 0; i < string.size(); i++)
		{
			CharPixel c = {fg, bg, string[i]};
			PutChar(x + i, y, c);
		}

		return true;
	}

	//Fills the framebuffer with a color
	bool Window::Fill(Color color)
	{
		CharPixel charPixel{ color, color, (char)223 };
		for (size_t y = 0; y < height / 2; y++)
		{
			for (size_t x = 0; x < width; x++)
			{
				frameBuffer[y * width + x] = charPixel;
			}
		}
		return true;
	}

	//Clear the depthbuffer, setting everything to 0
	bool Window::ClearDepthBuffer()
	{
		for (size_t y = 0; y < height; y++)
		{
			for (size_t x = 0; x < width; x++)
			{
				depthBuffer[y * width + x] = 0;
			}
		}
		return true;
	}

	//Get a pointer to the depth buffer bit of a pixel, returns nullptr on failure
	double* Window::GetDepthBufferBit(uint16_t x, uint16_t y)
	{
		//Make sure the pixel is in bounds
		if (x >= width || y >= height)
			return nullptr;

		return &depthBuffer[y * width + x];
	}

	//Set the properties of this window, clears the framebuffer
	bool Window::SetProperties(WindowProperties properties)
	{
		//Make sure the window is not sized too big
		if (properties.width > maxWidth || properties.height > maxHeight)
		{
			LogWarning("CVid warning in Window: Window dimensions too large, maximum is " + std::to_string(maxWidth) + ", " + std::to_string(maxHeight));
			return false;
		}

		//Send it to the console app
		if (!SendData(&properties, sizeof(properties), DataType::Properties))
			return false;

		//Round height to upper multiple of 2
		properties.height += properties.height % 2;

		width = properties.width;
		height = properties.height;

		//Resize the framebuffer and depth buffer
		delete[] frameBuffer;
		delete[] depthBuffer;
		frameBuffer = new CharPixel[width * height / 2];
		depthBuffer = new double[width * height];

		return true;
	}

	//Draw the current framebuffer
	bool Window::DrawFrame()
	{
		ClearDepthBuffer();

		const size_t frameSize = (size_t)width * (height / 2);

		return SendData(frameBuffer, frameSize * sizeof(CharPixel), DataType::Frame);
	}

	//Send data to the window process
	bool Window::SendData(const void* data, size_t amount, DataType type, bool block)
	{
		if (!alive)
			return false;

		//Make sure the window is still active
		DWORD code;
		if (!IsAlive(&code))
		{
			LogWarning("CVid warning in Window: Window exited unexpectedly, code " + std::to_string(code));

			return false;
		}

		//Block untill app sends a ready status if specified
		if (block)
		{
			//Read the status data
			char buffer[8];
			DWORD numBytesRead = 0;
			bool readPipeSuccess = ReadFile(
				inPipe,
				buffer, //The destination for the data from the pipe
				1 * sizeof(char), //Attempt to read this many bytes
				&numBytesRead,
				NULL //Not using overlapped IO
			);
		}

		//Prefix the data with it's type
		char* cdata = new char[amount + 1];
		cdata[0] = (char)type;
		memcpy(cdata + 1, data, amount);

		//Send the data
		bool sendSuccess = WriteFile(
			outPipe,
			cdata,
			(amount + 1) * sizeof(char), //How many bytes to send
			NULL, NULL //Irrelevant
		);

		delete[] cdata;

		//Make sure the data was sent
		if (!sendSuccess)
		{
			LogWarning("CVid warning in Window: Failed to send data to window, code " + std::to_string(GetLastError()));
			return false;
		}
		return true;
	}

	//Closes the window process
	void Window::CloseWindow()
	{
		if (alive)
		{
			//Close all handles.
			CloseHandle(processInfo.hProcess);
			CloseHandle(processInfo.hThread);
			CloseHandle(outPipe);

			//Call onClose if applicable
			if (onClose)
				onClose(this);

			alive = false;

			delete[] frameBuffer;
			delete[] depthBuffer;
		}
	}

	//Return true if the window process is still active
	bool Window::IsAlive(DWORD* exitCode)
	{
		if (alive)
		{
			DWORD code;
			GetExitCodeProcess(processInfo.hProcess, &code);
			//If not alive close the handles
			if (code != STILL_ACTIVE)
			{
				CloseWindow();
			}
			if (exitCode)
				*exitCode = code;
		}

		return alive;
	}

	//Get the dimensions of this window. Y is in pixel coordinates
	Vector2Int Window::GetDimensions()
	{
		return Vector2Int(width, height);
	}
}