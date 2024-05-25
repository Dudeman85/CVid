#include <cvid/Window.h>

int main()
{
	cvid::Window window = cvid::Window(40, 30, "ass");
	cvid::Window* window2 = new cvid::Window(30, 80, "testwindowssadsasdwfhhfdhdrhrddvbdfghfd2968402132*/-+");

	int c = 0;
	bool s = false;
	while (true)
	{
		c++;
		if (c > 100000000 && !s)
		{
			window2->SetProperties({ 240, 126 });
			s = true;
		}
		int a = sqrt(3333);
	}

	return 0;
}