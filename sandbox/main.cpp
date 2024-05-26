#include <iostream>
#include <fstream>
#include <chrono>
#include <cvid/Window.h>

using namespace std;

//Convenient Argparser functions
char* GetOption(char** argv, int argc, const string& option, const string& altName = "")
{
	//Find the option 
	char** itr = std::find(argv, argv + argc, option);
	if (itr == argv + argc)
		itr = std::find(argv, argv + argc, altName);

	//Make sure the arg after the option is valid
	if (itr != argv + argc && ++itr != argv + argc)
		return *itr;

	return 0;
}
bool OptionExists(char** argv, int argc, const string& option, const string& altName = "")
{
	//Check both normal and alt names
	bool exists = std::find(argv, argv + argc, option) != argv + argc;
	if (altName != "")
		exists |= std::find(argv, argv + argc, altName) != argv + argc;
	return exists;
}

struct VideoProperties
{
	unsigned short width = 0;
	unsigned short height = 0;
	unsigned short frames = 0;
	cvid::byte fps = 0;
};

//Load an image or video from a binary file
vector<cvid::byte> LoadData(const string& path, VideoProperties* properties)
{
	//Open the file in binary mode and seek to end
	ifstream ifs(path, ios::binary | ios::ate);

	//Make sure the file can be opened
	if (!ifs)
	{
		cout << "Could not load data from " + path;
		throw runtime_error("Error loading data from " + path);
	}

	//Save end point and go back to start
	streampos end = ifs.tellg();
	ifs.seekg(0, ios::beg);

	//Create a byte vector for the entire file data
	size_t size = size_t(end - ifs.tellg());
	vector<cvid::byte> data(size);

	//Copy the data from filestream to vector
	ifs.read((char*)data.data(), data.size());

	//Get the 2-byte dimension, frame count, and fps data
	properties->width = data[0] << 8;
	properties->width |= data[1];
	properties->height = data[2] << 8;
	properties->height |= data[3];
	properties->frames = data[4] << 8;
	properties->frames |= data[5];
	properties->fps = data[6];

	//Remove the dimension, frame count, and fps data
	data.erase(data.begin(), data.begin() + 7);

	return data;
}

int main(int argc, char* argv[])
{
	//Get the video name
	string videoName;
	if (OptionExists(argv, argc, "-v", "--video"))
	{
		//From command line args
		videoName = GetOption(argv, argc, "-v", "--video");
	}
	else
	{
		//Ask directly
		cout << "Enter the name of the video to play: ";
		cin >> videoName;
	}

	//ASCII characters to draw pixels with, each character has an upper and lower pixel
	char* characters = new char[4];
	//Load defaults
	characters[0] = (char)32;  //Empty
	characters[1] = (char)223; //Top
	characters[2] = (char)220; //Bottom
	characters[3] = (char)219; //Both
	//Load from command line if option was given
	if (OptionExists(argv, argc, "-c", "--charset"))
	{
		//Load from command line args
		char* charset = GetOption(argv, argc, "-c", "--charset");
		if (strlen(charset) >= 3)
		{
			characters[1] = charset[0]; //Top
			characters[2] = charset[1]; //Bottom
			characters[3] = charset[2]; //Both
		}
	}

	//Load the video data from file
	VideoProperties properties;
	vector<cvid::byte> videoData = LoadData(videoName + ".cvid", &properties);

	//Make the window
	cvid::Window window = cvid::Window(properties.width, properties.height, "CVid");

	//Calculate the time to wait between frames
	auto waitTime = chrono::microseconds((int)((1.f / properties.fps) * 1000000));
	unsigned int dataIndex = 0;
	unsigned int currentBit = 0;
	//Should a pixel be drawn
	bool drawState = false;
	//For each frame, currently max of 65535
	for (unsigned short frame = 0; frame < properties.frames; frame++)
	{
		auto frameStart = chrono::high_resolution_clock::now();

		//Draw the frame data
		//Every character in the frame is added to this string
		string frameString = "";

		//For each row going two at a time because each character represents two pixel rows
		for (unsigned short y = 0; y < properties.height; y += 2)
		{
			//For each column
			for (unsigned short x = 0; x < properties.width; x++)
			{
				cvid::byte toDraw = 0b00000000;
				//For both upper pixel and lower pixel if it exists
				for (char i = 1; i < (y == properties.height - 1 ? 2 : 3); i++)
				{
					//Check if we should change the drawState
					if (currentBit >= videoData[dataIndex])
					{
						dataIndex++;
						currentBit = 0;
						drawState = !drawState;
					}
					//If the data byte is 0 flip the state again
					if (videoData[dataIndex] == 0)
					{
						dataIndex++;
						currentBit = 0;
						drawState = !drawState;
					}
					//Check the pixel
					if (drawState)
						toDraw |= i;
					currentBit++;
				}
				//Add the right pixel to the frame string
				frameString += characters[toDraw];
			}
		}

		//Draw the frame
		window.SendData(frameString.c_str(), frameString.size(), cvid::DataType::Frame);
		//Move the cursor to 0, 0
		window.SendData("\x1b[0;0H", 7, cvid::DataType::Frame);

		//Keep a steady FPS regardless of processing time
		while (true)
		{
			if (waitTime < chrono::high_resolution_clock::now() - frameStart)
				break;
		}
	}

	return 0;
}
