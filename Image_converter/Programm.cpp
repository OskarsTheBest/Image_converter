#include <iostream>
#include "Window.h"

int main()
{
	std::cout << "Creating Window\n";
	
	Window* pWindow = new Window();


	while (pWindow->ProcessMessages())
	{

		// Window Render
		Sleep(10); // dont take too much power
	}
	std::cout << "Closing Window\n";
	delete pWindow;

	return 0;
}