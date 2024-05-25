#include <iostream>
#include <windows.h>
#include <cvid/Helpers.h>

int main(int argc, char* argv[])
{
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

	//Read the initial setup data
	char buffer[128];
	DWORD numBytesRead = 0;
	bool readPipeSuccess = ReadFile(
		pipe,
		buffer, //The destination for the data from the pipe
		127 * sizeof(wchar_t), //Attempt to read this many bytes
		&numBytesRead,
		NULL //Not using overlapped IO
	);

	//Make sure the read succeeded
	if (!readPipeSuccess || numBytesRead == 0)
	{
		cvid::LogError("CVid error in create window: Failed to read from pipe, error " + std::to_string(GetLastError()));
		system("pause");
		return -2;
	}

	//Null terminate the string
	buffer[numBytesRead / sizeof(wchar_t)] = '\0';
	std::cout << "Read " << numBytesRead << " bytes of data.";
	std::cout << buffer;

	//Display loop
	while (true)
	{
		//Read the initial setup data
		wchar_t buffer[128];
		DWORD numBytesRead = 0;
		bool readPipeSuccess = ReadFile(
			pipe,
			buffer, //The destination for the data from the pipe
			127 * sizeof(wchar_t), //Attempt to read this many bytes
			&numBytesRead,
			NULL //Not using overlapped IO
		);

		//Make sure the read succeeded
		if (!readPipeSuccess || numBytesRead == 0)
		{
			cvid::LogError("CVid error in create window: Failed to read from pipe, error " + std::to_string(GetLastError()));
			system("pause");
			return -2;
		}

		//Null terminate the string
		buffer[numBytesRead / sizeof(wchar_t)] = '\0';
		std::wcout << buffer;
	}
}