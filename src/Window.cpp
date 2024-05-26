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

		//Create the pipe to the new console process
		unsigned int id = std::this_thread::get_id()._Get_underlying_id();
		pipeName = std::format("\\\\.\\pipe\\process{}window{}", id, numWindowsCreated);
		pipe = CreateNamedPipeA(
			pipeName.c_str(),
			PIPE_ACCESS_OUTBOUND,
			PIPE_TYPE_MESSAGE | PIPE_READMODE_BYTE | PIPE_WAIT,
			1, //Max instances
			8192, //Output buffer size 
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

	//Send data to the window
	const bool Window::SendData(const char* data, size_t amount, Window::DataType type)
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
		cdata[0] = (byte)type;
		memcpy(cdata + 1, data, amount);

		//Send the data
		DWORD numBytesWritten = 0;
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

	//Set the properties of this window
	bool Window::SetProperties(WindowProperties properties)
	{
		//Make sure the window is to sized too big
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
		SendData(data, sizeof(properties), Window::DataType::Properties);

		return true;
	}
}