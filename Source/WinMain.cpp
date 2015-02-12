
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

//For memory leak detection
#define _CRT_DEBUG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

//My includes
#include "Input.h"
#include "NiteShader.h"
#include "Globals.h"
#include "Camera.h"

//DirectX includes
#include <d3d9.h>

#define CLASSNAME L"MainWindow"
#define SCREENWIDTH		GetSystemMetrics(SM_CXSCREEN)
#define SCREENHEIGHT	GetSystemMetrics(SM_CYSCREEN)

//Application instance
HINSTANCE	g_hInstance;
//Pointer to the window
HWND		g_hWnd;
//Global input class
Input*		g_Input;
//Camera class
Camera*		g_Camera;
//Application Class
NiteShader	g_NiteShader;

//Initializes the application window
void InitWindow();
//Callback to process the messages for the window
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//Dumps memory leaks from ALL exit points in debug mode
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	//Set global app pointer to the app instance
	g_hInstance = hInstance;

	//Initialize window
	InitWindow();

	//Initialize input
	Input input(DISCL_FOREGROUND | DISCL_NONEXCLUSIVE, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE, g_hInstance, g_hWnd);
	g_Input = &input;
	//Initialize Camera
	Camera camera;
	g_Camera = &camera;
	g_Camera->GetPos() = D3DXVECTOR3(0.f, 0.f, -20.f);
	//Initialize NiteShader
	g_NiteShader.Init();
	//My message structure to capture messages
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	//Set up the application timer -- get ticks per second
	__int64 cntsPerSec = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)&cntsPerSec);
	//seconds per tick is inverse of ticks per second
	float secPerCnt = 1.0f/(float)cntsPerSec;

	//Get frame begin time
	__int64 prevTimeStamp = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&prevTimeStamp);
	//for use as the current time stamp in main loop and delta time, declared here so memory isnt created and destroyed every frame
	__int64 currentTimeStamp = 0;
	float dt = 0.f;
	char buffer [128];
	int FPS = 0;
	int numFrames = 0;
	float frameTimer = 0.f;

	//Main loop
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (!g_NiteShader.IsDeviceLost())
		{
			//Get Current time stamp
			QueryPerformanceCounter((LARGE_INTEGER*)&currentTimeStamp);
			dt = (currentTimeStamp - prevTimeStamp) * secPerCnt;

			//Poll the input
			g_Input->Poll();
			g_NiteShader.Update(dt);
			g_Camera->Update(dt);
			g_NiteShader.Render(dt);

			if (frameTimer > 1.f)
			{
				FPS = int(numFrames);
				numFrames = 0;
				frameTimer = 0.f;
			}
			else 
			{
				numFrames++;
				frameTimer += dt;
			}

			sprintf_s(buffer, "Thomas Loudiana - NiteShader FPS: %i", FPS);
			SetWindowTextA(g_hWnd, buffer);

			//Update beginning frame stamp
			prevTimeStamp = currentTimeStamp;
		}
	}

	//Unregister the main window
	UnregisterClass(CLASSNAME, g_hInstance);

	g_NiteShader.Shutdown();

	//Quit
	return (int)msg.wParam;
}


//Initializes the application window
void InitWindow()
{
	//Declare window class and zero it out
	WNDCLASSEX wndClass;
	ZeroMemory(&wndClass, sizeof(WNDCLASSEX));

	//Set window parameters
	wndClass.cbSize			= sizeof(WNDCLASSEX);
	wndClass.style			= CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc	= (WNDPROC)WndProc;
	wndClass.hInstance		= g_hInstance;
	wndClass.hIcon			= NULL;
	wndClass.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground	= (HBRUSH)COLOR_WINDOWFRAME;
	wndClass.lpszClassName	= CLASSNAME;
	wndClass.hIconSm		= NULL;

	//Register the class in the atom table
	RegisterClassEx(&wndClass);

	//Create the window
	DWORD style = WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU;
	g_hWnd = CreateWindow(CLASSNAME, L"Thomas Loudiana - NiteShader",
						style, CW_USEDEFAULT, CW_USEDEFAULT,
						SCREENWIDTH, SCREENHEIGHT, NULL, NULL, g_hInstance, NULL);

	//Display the window
	ShowWindow(g_hWnd, SW_SHOW);
	UpdateWindow(g_hWnd);
}

//Processes messages for the main window
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//Handle Messages
	switch (msg)
	{
		//No message -- just repaint
		case (WM_PAINT) :
			InvalidateRect(g_hWnd, NULL, TRUE);
			break;
		//Window closes have it send a WM_DESTROY message
		case (WM_CLOSE) :
			DestroyWindow(g_hWnd);
			break;
		//Quit the app
		case (WM_DESTROY) :
			PostQuitMessage(0);
			break;
	}
	
	//Handle messages that the switch statement does not
	return DefWindowProc(hwnd, msg, wParam, lParam);
}
