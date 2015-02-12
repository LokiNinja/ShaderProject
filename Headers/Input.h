#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#define KEYDELAY .3f

//Handles application input
class Input
{
public:
	//Constructor/Destructor
	Input(DWORD keyboardCoopFlags, DWORD mouseCoopFlags, HINSTANCE hInstance, HWND hWnd);
	~Input();

	//Polls for input
	void Poll();
	//Is the key down? Note: must pass it as unsigned here so the value ranges from 0-255 instead of -128-127 
	bool KeyDown(unsigned char key);
	//Checks if the mouse button is down
	bool MouseButtonDown(int button);
	//Returns position changes for the mouse
	void MousePos(float &dx, float &dy, float &dz);
private:
	//Prevent copying
	Input(const Input&);
	Input& operator=(const Input&);
protected:
	//Direct Input object for querying devices and such
	IDirectInput8*			m_pDI;

	//Keyboard device
	IDirectInputDevice8*	m_pKeyboard;
	char					m_keyboardState[256];

	//Mouse device
	IDirectInputDevice8*	m_pMouse;
	DIMOUSESTATE2			m_mouseState;
};

extern Input* g_Input;