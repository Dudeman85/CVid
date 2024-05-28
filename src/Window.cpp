#include <cvid/Window.h>
#include <cvid/Helpers.h>
#include <thread>
#include <format>

namespace cvid
{
	//Create a new console window
	Window::Window(int width, int height, std::string name)
	{
		//Setup some basic variables
		this->properties.width = width;
		this->properties.height = height;
		this->name = name;
		HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
		maxWidth = GetLargestConsoleWindowSize(console).X;
		maxHeight = GetLargestConsoleWindowSize(console).Y * 2;

		//Size the framebuffer
		framebuffer.resize(width);
		for (auto& col : framebuffer)
		{
			col.resize(height);
			for (auto& row : col)
			{
				row = Color::Black;
			}
		}

		//Create the pipe to the new console process
		unsigned int id = std::this_thread::get_id()._Get_underlying_id();
		pipeName = std::format("\\\\.\\pipe\\process{}window{}", id, numWindowsCreated);
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
		//Terminate the window process
		TerminateProcess(processInfo.hProcess, 0);

		//Close all handles. 
		CloseHandle(processInfo.hProcess);
		CloseHandle(processInfo.hThread);
		CloseHandle(pipe);
	}

	//Set a pixel on the framebuffer to some color, returns true on success
	bool Window::PutPixel(int x, int y, Color color)
	{
		if (x >= properties.width || y >= properties.height)
		{
			LogWarning("CVid warning in PutPixel: Position out of range");
			return false;
		}

		framebuffer[x][y] = color;

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

		this->properties = properties;

		//Get the properties struct into binary form
		char data[sizeof(properties)];
		memcpy(&data, &properties, sizeof(properties));

		//Send it to the console app
		SendData(data, sizeof(properties), DataType::Properties);

		return true;
	}

	//Draw the current framebuffer
	bool Window::DrawFrame()
	{
		/*
		Pixels are formatted two above each other in one character
		When the pixels are different we will always print 223 where foreground is the top and background is the bottom.
		When they differ we will print 219 where foreground will be the color.
		*/

		//Format (8 bytes): \x1b[<color>m<char>
		std::string displayFrame;
		displayFrame.reserve((size_t)8 * properties.width * properties.height / 2);

		//Cursor to 0, 0
		displayFrame.append("\x1b[0;0H");

		//For every pixel in the framebuffer
		for (size_t y = 0; y < properties.height; y += 2)
		{
			for (size_t x = 0; x < properties.width; x++)
			{
				std::string pixelString;
				if (framebuffer[x][y] == framebuffer[x][y + 1])
				{
					//If the upper and lower pixel are the same we print 219
					pixelString = std::format("\x1b[{}m{}", std::to_string(framebuffer[x][y]), (char)219);
				}
				else
				{
					//If the upper and lower pixel are different same we print 223
					pixelString = std::format("\x1b[{};{}m{}", std::to_string(framebuffer[x][y]), std::to_string(framebuffer[x][y + 1] + 10), (char)223);
				}

				//Add the proper vts to the displayFrame
				displayFrame.append(pixelString);
			}
		}

		return SendData(displayFrame.c_str(), displayFrame.length(), DataType::String);
	}

	//Send data to the window process
	bool Window::SendData(const char* data, size_t amount, DataType type)
	{
		if (!active)
			return false;

		//Make sure the window is still active
		DWORD code;
		GetExitCodeProcess(processInfo.hProcess, &code);
		if (code != STILL_ACTIVE)
		{
			LogWarning("CVid warning in Window: Window exited unexpectedly, code " + std::to_string(code));

			//Close all handles. 
			CloseHandle(processInfo.hProcess);
			CloseHandle(processInfo.hThread);
			CloseHandle(pipe);

			//Call onClose if applicable
			if (onClose)
				onClose(this);
			active = false;

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
}