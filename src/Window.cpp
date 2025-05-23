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
	Window::Window(uint16_t width, uint16_t height, std::string name, bool newProcess)
	{
		//Round height to upper multiple of 2
		height += height % 2;
		//Setup some basic variables
		this->width = width;
		this->height = height;
		this->name = name;
		this->seperateProcess = newProcess;
		numWindowsCreated++;

		//Create the frame and depth buffers
		frameBuffer = new CharPixel[(size_t)width * height / 2];
		depthBuffer = new double[(size_t)width * height];
		ClearDepthBuffer();

		//Create a new console window process if requested, otherwise usurp the main console
		if (newProcess)
			CreateAsNewProcess(name);
		else
			CreateAsMain(name);

		//Set the properties of the Window
		Resize(width, height);
	}

	//Create this window using the main application's console
	void Window::CreateAsMain(std::string name)
	{
		//Get the console handle
		consoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
		consoleIn = GetStdHandle(STD_INPUT_HANDLE);

		//Save the original window stats
		CONSOLE_SCREEN_BUFFER_INFO origInfo;
		GetConsoleScreenBufferInfo(consoleOut, &origInfo);
		originalSize = origInfo.srWindow;
		originalSbSize = origInfo.dwSize;
		GetConsoleTitle(originalTitle, sizeof(originalTitle));

		//Enable virtual terminal processing
		SetConsoleMode(consoleOut, ENABLE_VIRTUAL_TERMINAL_PROCESSING | ENABLE_PROCESSED_OUTPUT);
		//Disable quick edit
		originalMode = 0;
		GetConsoleMode(consoleIn, &originalMode);
		SetConsoleMode(consoleIn, (originalMode & ~ENABLE_QUICK_EDIT_MODE) | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT);
		//Rename window
		SetConsoleTitle(name.c_str());

		//Hide the cursor
		std::cout << "\x1b[?25l";
	}

	//Create this window as a new process
	void Window::CreateAsNewProcess(std::string name)
	{
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
	}

	Window::~Window()
	{
		CloseWindow();
	}

	//Set a pixel on the framebuffer to some color, returns true on success
	bool Window::PutPixel(Vector2Int pos, Color color)
	{
		return PutPixel(pos.x, pos.y, color);
	}
	//Set a pixel on the framebuffer to some color, returns true on success
	bool Window::PutPixel(uint16_t x, uint16_t y, Color color)
	{
		//Make sure the pixel is in bounds
		if (x >= width || y >= height)
			return false;

		//Pixels are formatted two above each other in one character
		//We will always print 223 where foreground is the top and background is the bottom.
		CharPixel& thisPixel = frameBuffer[((height - 1 - y) / 2) * width + x];

		//Set the pixel character
		thisPixel.character = (char)223;
		//Top or bottom pixel
		if (y % 2 == 0)
			thisPixel.bg = color;
		else
			thisPixel.fg = color;

		return true;
	}
	//Set a pixel on the framebuffer to some color, implements depth buffer, returns true on success
	bool Window::PutPixel(Vector2Int pos, Color color, double z)
	{
		return PutPixel(pos.x, pos.y, color, z);
	}
	//Set a pixel on the framebuffer to some color, implements depth buffer, returns true on success
	bool Window::PutPixel(uint16_t x, uint16_t y, Color color, double z)
	{
		//Make sure the pixel is in bounds
		if (x >= width || y >= height || z < 0)
			return false;

		//Make sure there is not already a closer pixel
		if (enableDepthTest)
		{
			//Basically smaller z means further away
			if (z - depthBuffer[y * width + x] > 0.5)
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
			thisPixel.bg = color;
		else
			thisPixel.fg = color;

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
			LogWarning("CVid warning in PutChar: Position out of range");
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
			CharPixel c = { fg, bg, string[i] };
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

	//Clear the depthbuffer, setting everything to -1
	bool Window::ClearDepthBuffer()
	{
		for (size_t y = 0; y < height; y++)
		{
			for (size_t x = 0; x < width; x++)
			{
				depthBuffer[y * width + x] = INFINITY;
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
	bool Window::Resize(int16_t w, int16_t h)
	{
		//Make sure the window is not sized too big
		Vector2Int maxSize = MaxWindowSize();
		if (w > maxSize.x || h > maxSize.y)
		{
			LogWarning("CVid warning in Window: Window dimensions too large, maximum is " + std::to_string(maxWidth) + ", " + std::to_string(maxHeight));
			return false;
		}

		//Round height to upper multiple of 2
		h += h % 2;
		width = w;
		height = h;

		//Send it to the console app
		if (seperateProcess)
		{
			WindowProperties properties{ width, height };
			if (!SendData(&properties, sizeof(properties), DataType::Properties))
				return false;
		}
		else
		{
			//Apply them straight to the main process console
			ResizeMain(width, height);
		}

		//Resize the framebuffer and depth buffer
		delete[] frameBuffer;
		delete[] depthBuffer;
		frameBuffer = new CharPixel[width * height / 2];
		depthBuffer = new double[width * height];

		return true;
	}

	//Resize the console to fit the frame
	void Window::ResizeMain(int16_t w, int16_t h)
	{
		SMALL_RECT minSize{ 0, 0, 1, 1 };
		SMALL_RECT consoleSize{ 0, 0, w - 1, (short)ceil((float)h / 2) - 1 };
		COORD sbSize{ w, (short)ceil((float)h / 2) };

		//SetConsoleWindowInfo has to be called before and after SetConsoleScreenBufferSize othewise Windows has a fit
		SetConsoleWindowInfo(consoleOut, true, &minSize);
		if (!SetConsoleScreenBufferSize(consoleOut, sbSize))
		{
			cvid::LogError("CVid error in Window: Failed to set console screen buffer size. Code " + std::to_string(GetLastError()));
			throw "Failed to set console screen buffer size";
		}
		if (!SetConsoleWindowInfo(consoleOut, true, &consoleSize))
		{
			cvid::LogError("CVid error in Window: Failed to set console size. Code " + std::to_string(GetLastError()));
			throw "Failed to set console screen buffer size";
		}
	}

	//Draw the current framebuffer
	bool Window::DrawFrame()
	{
		ClearDepthBuffer();

		const size_t frameSize = (size_t)width * (height / 2);

		//Send to process if seperate
		if (seperateProcess)
			return SendData(frameBuffer, frameSize * sizeof(CharPixel), DataType::Frame);
		//Draw the frame directly
		else
		{
			std::string frameString;
			frameString.reserve((size_t)29 * width * (height / 2));

			//Set colors to black
			std::cout << "\x1b[38;2;0;0;0m\x1b[48;2;0;0;0m";

			cvid::Color currentBg{ 0, 0, 0 };
			cvid::Color currentFg{ 0, 0, 0 };
			//For every pixel in the framebuffer
			for (size_t y = 0; y < height / 2; y++)
			{
				//Windows 11 broke text wrapping, so do we it here. Also for some reason it starts from 1
				frameString.append(std::format("\x1b[{};0f", y + 1));

				for (size_t x = 0; x < width; x++)
				{
					cvid::CharPixel& thisPixel = frameBuffer[y * width + x];

					//Change foreground color if it changes
					if (thisPixel.fg.r != currentFg.r || thisPixel.fg.g != currentFg.g || thisPixel.fg.b != currentFg.b)
					{
						//Add the proper vts to the displayFrame
						//Format: \x1b38;2;<r>;<g>;<b>;m
						frameString.append(std::format("\x1b[38;2;{};{};{}m", thisPixel.fg.r, thisPixel.fg.g, thisPixel.fg.b));
						currentFg = thisPixel.fg;
					}
					//Change background color if it changes
					if (thisPixel.bg.r != currentBg.r || thisPixel.bg.g != currentBg.g || thisPixel.bg.b != currentBg.b)
					{
						//Add the proper vts to the displayFrame
						//Format: \x1b48;2;<r>;<g>;<b>;m
						frameString.append(std::format("\x1b[48;2;{};{};{}m", thisPixel.bg.r, thisPixel.bg.g, thisPixel.bg.b));
						currentBg = thisPixel.bg;
					}

					frameString += thisPixel.character;
				}
			}

			//Print the frame
			std::cout << frameString;
			return true;
		}
	}

	//Send data to the window process
	bool Window::SendData(const void* data, size_t amount, DataType type, bool block)
	{
		if (!alive)
			return false;

		//Only string works with main window
		if (!seperateProcess)
		{
			if (type == DataType::String)
			{
				std::cout << (char*)data;
				return true;
			}
			else
			{
				return false;
			}
		}

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
		if (!alive)
			return;
		if (seperateProcess)
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
		else
		{
			system("cls");
			//Reset Color and show cursor
			std::cout << "\x1b[38;2;204;204;204m\x1b[48;2;12;12;12m\x1b[?25h";
			//Restore original properties
			SMALL_RECT minSize{ 0, 0, 1, 1 };
			SetConsoleWindowInfo(consoleOut, true, &minSize);
			SetConsoleScreenBufferSize(consoleOut, originalSbSize);
			SetConsoleWindowInfo(consoleOut, true, &originalSize);
			SetConsoleMode(consoleIn, originalMode);
			SetConsoleTitle(originalTitle);
		}
	}

	//Return true if the window process is still active
	bool Window::IsAlive(DWORD* exitCode)
	{
		if (alive && seperateProcess)
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

	//Get the input record of this console window
	std::vector<INPUT_RECORD> Window::GetInputRecord()
	{
		//Get the input from console
		INPUT_RECORD inputRecord[128];
		DWORD numRead;
		PeekConsoleInput(consoleIn, inputRecord, 128, &numRead);
		FlushConsoleInputBuffer(consoleIn);

		//Put it in a vector for ease of use
		std::vector<INPUT_RECORD> recordVec;
		recordVec.reserve(numRead);
		for (size_t i = 0; i < numRead; i++)
		{
			recordVec.push_back(inputRecord[i]);
		}

		return recordVec;
	}

	//Get the dimensions of this window. Y is in pixel coordinates
	Vector2Int Window::GetSize()
	{
		return Vector2Int(width, height);
	}
}