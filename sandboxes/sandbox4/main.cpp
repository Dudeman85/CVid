#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <cvid/Window.h>

#define byte uint8_t

constexpr uint8_t width = 96;
constexpr uint8_t height = 64;

using namespace std;

uint16_t frameCount = 0;
uint16_t frame = 0;

//Load an image or video from a binary file
vector<byte> LoadData(const string& path)
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
	vector<byte> data(size);

	//Copy the data from filestream to vector
	ifs.read((char*)data.data(), data.size());

	//Get the 2-byte frame count
	frameCount = (data[1] << 8) | data[0];

	return data;
}

bool* testFrameBuffer = new bool[width * height];

//Helper for painting the next pixel
int xPos = 0;
int yPos = 0;
void ShiftHead(int amount)
{
	//Next scanline
	xPos += amount;
	while (xPos >= width)
	{
		xPos -= width;
		yPos++;
	}
	if (yPos >= height)
	{
		yPos -= height;
	}
}
size_t changedPixels = 0;
void PutNextPixel(cvid::Color col, cvid::Window& window)
{
	bool c = col == cvid::Color::Black ? true : false;
	if (testFrameBuffer[yPos * width + xPos] != c)
		changedPixels++;
	testFrameBuffer[yPos * width + xPos] = c;

	window.PutPixel({ xPos, yPos }, col);

	ShiftHead(1);
}

int main()
{
	//Test framebuffer
	for (size_t i = 0; i < (size_t)width * height; i++)
	{
		testFrameBuffer[i] = 0;
	}


	cvid::Window window(width, height, "Decompression test");
	window.enableDepthTest = false;

	vector<byte> data = LoadData("C:\\Users\\Aleksi\\Desktop\\TIASM\\programs\\video\\BadApple Uncompressed.cvid");
	//Start after frame count
	size_t readHead = 2;

	//Fill the screen with white
	window.Fill(cvid::Color::White);

	//FPS
	auto waitTime = chrono::microseconds((int)((1.f / 158888888888888) * 1000000));

	//For each frame in the video
	for (frame = 0; frame < frameCount; frame++)
	{
		cout << "Start frame " << frame << endl;
		auto frameStart = chrono::high_resolution_clock::now();

		//For each bit
		for (size_t i = 0; i < ((size_t)width * height) / 8; i++)
		{
			//Read the byte
			byte pixel = data[readHead++];

			for (int bitmask = 0b10000000; bitmask > 0; bitmask >>= 1)
			{
				if (pixel & bitmask)
					PutNextPixel(cvid::Color::Black, window);
				else
					PutNextPixel(cvid::Color::White, window);
			}
		}

		//Display the frame
		window.DrawFrame();
		window.SendData("\x1b[0;0H", 7, cvid::DataType::String);
		xPos = 0;
		yPos = 0;

		cout << "Drew frame " << frame << " of " << frameCount << endl;
		std::cout << changedPixels << endl;
		changedPixels = 0;

		//Keep a steady FPS regardless of processing time
		while (true)
		{
			if (waitTime < chrono::high_resolution_clock::now() - frameStart)
				break;
		}
	}


	return 0;
}