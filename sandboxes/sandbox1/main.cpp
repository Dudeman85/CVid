#include <iostream>
#include <fstream>
#include <chrono>
#include <cvid/Window.h>
#include <cvid/Helpers.h>

using namespace std;

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
	string videoName = "vids/badapple";

	//ASCII characters to draw pixels with, each character has an upper and lower pixel
	char* characters = new char[4];
	//Load defaults
	characters[0] = (char)32;  //Empty
	characters[1] = (char)223; //Top
	characters[2] = (char)220; //Bottom
	characters[3] = (char)219; //Both

	//Load the video data from file
	VideoProperties properties;
	vector<cvid::byte> videoData = LoadData(videoName + ".cvid", &properties);

	//Make the window
	cvid::Window window(properties.width, properties.height, "Bad Apple");
	cvid::Window window2(60, 60, "Color Test");

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
		window.SendData(frameString.c_str(), frameString.size(), cvid::DataType::String);
		//Move the cursor to 0, 0
		window.SendData("\x1b[0;0H", 7, cvid::DataType::String);

		//Put a random pixel on window2
		int x = rand() % 60;
		int y = rand() % 60;
		window2.PutPixel(x, y, cvid::RandomColor());
		window2.DrawFrame();

		//Keep a steady FPS regardless of processing time
		while (true)
		{
			if (waitTime < chrono::high_resolution_clock::now() - frameStart)
				break;
		}
	}

	return 0;
}
