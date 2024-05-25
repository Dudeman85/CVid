#include <cvid/Window.h>
#include <cvid/Helpers.h>

namespace cvid
{
	//Create a new console window
	Window::Window(int width, int height, std::string name)
	{
		this->width = width;
		this->height = height;
		this->name = name;

		//Create the pipe to the new console process
		pipeName = std::string("\\\\.\\pipe\\").append(name);
		pipe = CreateNamedPipeA(
			pipeName.c_str(),
			PIPE_ACCESS_OUTBOUND,
			PIPE_TYPE_MESSAGE |	PIPE_READMODE_BYTE | PIPE_WAIT,
			1, //Max instances
			4096, //Output buffer size 
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
			LogError("CVid error in Window : Failed to connect pipe, code " + std::to_string(GetLastError()));
			CloseHandle(pipe);
			throw std::runtime_error("Failed to connect pipe");
			return;
		}

		//Send the setup instructions
		const char* data = " ***Hello Pipe World***";
		DWORD numBytesWritten = 0;
		bool sendSuccess = WriteFile(
			pipe,
			data,
			strlen(data) * sizeof(wchar_t), //How many bytes to send
			NULL, NULL //Irrelevant
		);
		//Make sure the data was sent
		if (!sendSuccess)
		{
			LogWarning("CVid warning in Window : Failed to send data to window, code " + std::to_string(GetLastError()));
		}
	}

	Window::~Window()
	{
		//Close all handles. 
		CloseHandle(processInfo.hProcess);
		CloseHandle(processInfo.hThread);
		CloseHandle(pipe);
	}
}