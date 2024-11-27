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


DrawLine(x0, y0, x1, y1):
	//If slope is less than 1
	if abs(x1 - x0) > abs(y1 - y0):
		//Make sure x1 is smaller than x2
		if x0 > x1:
			swap(x0, x1)
			swap(y0, y1)
		//Slope is rise/run
		m = (y1 - y0) / (x1 - x0)
		y = y0
		//For each x position, plot the corresponding y
		for x from x0 to x1:
			PutPixel(x, y)
			y += m
	else:
		//Make sure y1 is smaller than y2
		if y0 > y1:
			swap(x0, x1)
			swap(y0, y1)
		//Slope is run/rise
		m = (y1 - y0) / (x1 - x0)
		x = x0
		//For each y position, plot the corresponding x
		for y from y0 to y1:
			PutPixel(x, y)
			x += m
		


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

//Helper for painting the next pixel
int xPos = 0;
int yPos = 0;
int squareX = 0;
int squareY = 0;
void ShiftHead(int amount)
{
	squareX += amount;
	if (squareX < 8)
	{
		xPos += amount;
	}

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
void PutNextPixel(cvid::ConsoleColor col, cvid::Window& window)
{
	window.PutPixel({ xPos, yPos }, col);
	ShiftHead(1);
}

int main()
{
	cvid::Window window(width, height, "Decompression test");
	window.enableDepthTest = false;

	vector<byte> data = LoadData("C:\\Users\\Aleksi\\Desktop\\TIASM\\programs\\video\\BadApple.cvid");
	//Start after frame count
	size_t readHead = 2;

	//Fill the screen with white
	window.Fill(cvid::ConsoleColor::White);

	//FPS
	auto waitTime = chrono::microseconds((int)((1.f / 15) * 1000000));

	//For each frame in the video
	for (frame = 0; frame < frameCount; frame++)
	{
		cout << "Start frame " << frame << endl;
		auto frameStart = chrono::high_resolution_clock::now();

		//For each set 1 bit (2)
		for (int set1bit = 0; set1bit < 2; set1bit++)
		{
			//Read the 2 bytes for set 1
			byte set1 = data[readHead++];

			//Shift the bitmask 8 times, stop after 8th bit
			for (int set1bitmask = 0b10000000; set1bitmask > 0; set1bitmask >>= 1)
			{
				//If the bit is set in set1
				if (set1 & set1bitmask)
				{
					//Read the set2 byte
					byte set2 = data[readHead++];

					//Check each of the 8 bits with a bitmask
					for (int set2bitmask = 0b10000000; set2bitmask > 0; set2bitmask >>= 1)
					{
						//If the bit is set in set2
						if (set2 & set2bitmask)
						{
							//Read the set3 byte
							byte set3 = data[readHead++];

							//Special case for all black
							if (set3 & 0b10000000)
							{
								//Paint the next 8*6 pixels black
								for (size_t i = 0; i < 8 * 6; i++)
									PutNextPixel(cvid::ConsoleColor::Black, window);
								continue;
							}
							//Special case for all white
							if (set3 & 0b01000000)
							{
								//Paint the next 8*6 pixels white
								for (size_t i = 0; i < 8 * 6; i++)
									PutNextPixel(cvid::ConsoleColor::White, window);
								continue;
							}

							//Check each of the 8 bits with a bitmask
							for (int set3bitmask = 0b00100000; set3bitmask > 0; set3bitmask >>= 1)
							{
								//If the bit is set in set3
								if (set3 & set3bitmask)
								{
									//Read the set4 byte
									byte set4 = data[readHead++];

									//Check each of the 8 bits with a bitmask
									for (int set4bitmask = 0b10000000; set4bitmask > 0; set4bitmask >>= 1)
									{
										//If the bit is set in set3
										if (set4 & set4bitmask)
											PutNextPixel(cvid::ConsoleColor::Black, window);
										else
											PutNextPixel(cvid::ConsoleColor::White, window);
									}
								}
								else
								{
									//Shift head by 8 to compensate lack of set4
									ShiftHead(8);
								}
							}
						}
						else
						{
							//Shift head by 8 * 6 to compensate lack of set3
							ShiftHead(8 * 6);
						}
					}
				}
				else
				{
					//Shift head by 8 * 6 * 8 to compensate lack of set2
					ShiftHead(8 * 6 * 8);
				}
			}
		}

		//Display the frame
		window.DrawFrame();
		window.SendData("\x1b[0;0H", 7, cvid::DataType::String);

		cout << "Drew frame " << frame << " of " << frameCount << endl;
		cout << xPos << ", " << yPos << endl;
		xPos = 0;
		yPos = 0;

		//Keep a steady FPS regardless of processing time
		while (true)
		{
			if (waitTime < chrono::high_resolution_clock::now() - frameStart)
				break;
		}
	}

	return 0;
}