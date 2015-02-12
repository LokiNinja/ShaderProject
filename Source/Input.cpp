#include "Input.h"
#include "Globals.h"

//Constructor
Input::Input(DWORD keyboardCoop, DWORD mouseCoop, HINSTANCE hInstance, HWND hWnd)
{
	//Create DirectInput Object
	HR(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**) &m_pDI, 0));

	//Create the devices
	HR(m_pDI->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, 0));
	HR(m_pDI->CreateDevice(GUID_SysMouse, &m_pMouse, 0));

	//Set Data format
	HR(m_pKeyboard->SetDataFormat(&c_dfDIKeyboard));
	HR(m_pMouse->SetDataFormat(&c_dfDIMouse2));

	//Set Coop levels
	HR(m_pKeyboard->SetCooperativeLevel(hWnd, keyboardCoop));
	HR(m_pMouse->SetCooperativeLevel(hWnd, mouseCoop));

	//Acquire Devices
	m_pKeyboard->Acquire();
	m_pMouse->Acquire();
}

//Destructor
Input::~Input()
{
	SAFE_RELEASE(m_pDI);
	m_pKeyboard->Unacquire();
	m_pMouse->Unacquire();
	SAFE_RELEASE(m_pKeyboard);
	SAFE_RELEASE(m_pMouse);
}

//Polls the devices
void Input::Poll()
{
	//Used to check if devices need to be reacquired
	HRESULT hr;

	//Keyboard Poll
	hr = m_pKeyboard->GetDeviceState(sizeof(m_keyboardState), (void**)&m_keyboardState);

	//Polling failed reacquire
	if (FAILED(hr))
	{
		ZeroMemory(&m_keyboardState, sizeof(m_keyboardState));
		m_pKeyboard->Acquire();
	}

	//Mouse Poll
	hr = m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE2), (void**)&m_mouseState);

	//Polling failed reacquire
	if (FAILED(hr))
	{
		ZeroMemory(&m_mouseState, sizeof(DIMOUSESTATE2));
		m_pMouse->Acquire();
	}
}

//Checks the state of an individual key
bool Input::KeyDown(unsigned char key)
{
	return (m_keyboardState[key] & 0x80) != 0;
}

//Checks if a mouse button is down
bool Input::MouseButtonDown(int button)
{
	return (m_mouseState.rgbButtons[button] & 0x80) != 0;
}

//Returns the relative mouse positions
void Input::MousePos(float &dx, float &dy, float &dz)
{
	dx = (float)m_mouseState.lX;
	dy = (float)m_mouseState.lY;
	dz = (float)m_mouseState.lZ;
}

