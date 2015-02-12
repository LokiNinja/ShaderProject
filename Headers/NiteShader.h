#pragma once
#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <vector>
#include "Light.h"
using namespace std;

class Effect;

extern HWND g_hWnd;
extern HINSTANCE g_hInstance;

class Actor;

class NiteShader
{
public:
	//Constructor/Destructor
	NiteShader();
	~NiteShader();

	//Initializes NiteShader
	void Init();
	//Shutsdown
	void Shutdown();
	//Updates NiteShader
	void Update(float dt);
	//Renders a frame
	void Render(float dt);
	//Checks whether a device has been lost
	bool IsDeviceLost();
	//Cleans up com objects when a device is lost
	void OnLostDevice();
	//Sets up everything when a device is reset
	void OnResetDevice();
	//Prints a string to the screen
	void PrintString(char* name, RECT* rect);

	//Vector of all the effects
	vector<Effect*>		m_effectList;
	//Currently selected effect
	int					m_selectedEffect;
	//Get Device
	IDirect3DDevice9*	GetDevice()		const { return m_pD3DDevice; }
	//Get width		
	int					GetWidth()		const { return m_width; }
	//Get Height
	int					GetHeight()		const { return m_height; }
	//Gets the world matrix
	const D3DXMATRIX&	GetWorld()		const { return m_world; }
	//Returns the light for interaction with shaders
	Light*		GetLight() { return m_pLight; }


private:
	//No copying
	NiteShader(const NiteShader&);
	NiteShader& operator=(const NiteShader&);

	//Direct 3D object for enumerating devices and display modes
	IDirect3D9*				m_pD3D;
	//Direct 3d device for rendering
	IDirect3DDevice9*		m_pD3DDevice;
	//Stores the devices capabilities
	D3DCAPS9				m_D3DCaps;
	//Stores the preferred presentation parameters
	D3DPRESENT_PARAMETERS	m_D3DPP;
	//Clear Color
	D3DXCOLOR				m_clearColor;
	//Mesh object to render with effect
	Actor*					m_pActor;
	//Font object for printing effect name
	ID3DXFont*				m_pFont;

	//Light-------------------------------------------------------------------Start with just one, can change later
	Light*					m_pLight;

	//World transform
	D3DXMATRIX				m_world;

	//width and height of the window
	int m_width;
	int m_height;

	//HELPER FUNCTIONS

	//Initializes D3D
	void InitD3D();
	//Loads a mesh to display effects on, if null a sphere is created
	void InitActor(char* fileName);	
	//Loads all effects
	void LoadEffects();
};