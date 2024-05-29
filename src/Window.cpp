#include <cvid/Window.h>
#include <cvid/Helpers.h>
#include <thread>
#include <format>

namespace cvid
{
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

		//Size the framebuffer
		framebuffer = new CharPixel[width * (height / 2)];

		//Create the pipe to the new console process
		unsigned int pid = std::this_thread::get_id()._Get_underlying_id();
		pipeName = std::format("\\\\.\\pipe\\process{}window{}", pid, numWindowsCreated);
		pipe = CreateNamedPipeA(
			pipeName.c_str(),
			PIPE_ACCESS_OUTBOUND,
			PIPE_TYPE_MESSAGE | PIPE_READMODE_BYTE | PIPE_WAIT,
			1, //Max instances
			16386, //Output buffer size 
			0, //Input buffer size
			50, //Time out in ms
			NULL //Security attributes
		);
		//Make sure the pipe creation worked
		if (pipe == NULL || pipe == INVALID_HANDLE_VALUE)
		{
			LogError("CVid error in Window: Failed to create pipe, code " + std::to_string(GetLastError()));
			throw std::runtime_error("Failed to create pipe");
			return;
		}

		//Create an instance of the ConsoleWindowApp
		//Setup startup info
		STARTUPINFO startupInfo;
		ZeroMemory(&startupInfo, sizeof(startupInfo));
		startupInfo.cb = sizeof(startupInfo);
		startupInfo.lpTitle = const_cast<LPSTR>(name.c_str());
		ZeroMemory(&processInfo, sizeof(processInfo));

		//Create the process
		std::string cmdline("ConsoleWindowApp.exe ");
		cmdline.append(pipeName);
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

		//Connect the process via the pipe
		bool connectPipeSuccess = ConnectNamedPipe(pipe, NULL);
		//Make sure the pipe connected successfully
		if (!connectPipeSuccess)
		{
			LogError("CVid error in Window: Failed to connect pipe, code " + std::to_string(GetLastError()));
			CloseHandle(pipe);
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
	bool Window::PutPixel(uint16_t x, uint16_t y, Color color)
	{
		//Make sure the pixel is in bounds
		if (x >= width || y >= height)
		{
			LogWarning("CVid warning in PutPixel: Position out of range");
			return false;
		}

		//Pixels are formatted two above each other in one character
		//We will always print 223 where foreground is the top and background is the bottom.
		CharPixel& thisPixel = framebuffer[(y / 2) * width + x];

		//Set le pixel character
		thisPixel.character = (char)223;
		//Top or bottom pixel
		if (y % 2 == 1)
			thisPixel.foregroundColor = color;
		else
			thisPixel.backgroundColor = color;

		return true;
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

		framebuffer[y * width + x] = charPixel;

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
				framebuffer[y * width + x] = charPixel;
			}
		}
		return true;
	}

	//Set the properties of this window
	bool Window::SetProperties(WindowProperties properties)
	{
		//Make sure the window is not sized too big
		if (properties.width > maxWidth || properties.height > maxHeight)
		{
			LogWarning("CVid warning in Window: Window dimensions too large, maximum is " + std::to_string(maxWidth) + ", " + std::to_string(maxHeight));
			return false;
		}

		width = properties.width;
		height = properties.height;

		//Send it to the console app
		SendData(&properties, sizeof(properties), DataType::Properties);

		return true;
	}

	//Draw the current framebuffer
	bool Window::DrawFrame()
	{
		size_t frameSize = (size_t)width * (height / 2);

		return SendData(framebuffer, frameSize * sizeof(CharPixel), DataType::Frame);
	}

	//Send data to the window process
	bool Window::SendData(void* data, size_t amount, DataType type)
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

		//Prefix the data with it's type
		char* cdata = new char[amount + 1];
		cdata[0] = (char)type;
		memcpy(cdata + 1, data, amount);

		//Send the data
		bool sendSuccess = WriteFile(
			pipe,
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
		//Close all handles. 
		CloseHandle(processInfo.hProcess);
		CloseHandle(processInfo.hThread);
		CloseHandle(pipe);

		//Call onClose if applicable
		if (onClose)
			onClose(this);

		alive = false;
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
}