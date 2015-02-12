#include "NiteShader.h"
#include "Globals.h"
#include "Actor.h"
#include "AmbientEffect.h"
#include "DiffuseEffect.h"
#include "SpecularEffect.h"
#include "GoochEffect.h"
#include "PhongEffect.h"
#include "SubsurfaceScatterEffect.h"
#include "ShadowmapEffect.h"
#include "BumpEffect.h"
#include "ToonEffect.h"
#include "Input.h"
#include "Camera.h"

IDirect3DVertexDeclaration9*	Vertex::decl;

//Constructor
NiteShader::NiteShader()
{
	//Initialize pointers to 0
	m_pD3D = 0;
	m_pD3DDevice = 0;
	m_pFont = 0;
	m_pLight = 0;
	//Zero out structures and non pointers
	ZeroMemory(&m_D3DPP, sizeof(D3DPRESENT_PARAMETERS));
	ZeroMemory(&m_D3DCaps, sizeof(D3DCAPS9));
	m_width = 0;
	m_height = 0;
	m_clearColor = D3DXCOLOR(0.0f, 0.7f, 0.5f, 1.0f);
	m_pActor = 0;
	m_selectedEffect = 0;
	D3DXMatrixIdentity(&m_world);
}

//Destructor
NiteShader::~NiteShader()
{
	Shutdown();
}

//Initializes NiteShader
void NiteShader::Init()
{
	m_pLight = new Light();
	//Initialize D3D
	InitD3D();
	//Initialize Mesh Actor
	InitActor(NULL);
	//Load Effects
	LoadEffects();
}

//Performs Cleanup
void NiteShader::Shutdown()
{
	vector<Effect*>::iterator it = m_effectList.begin();
	//Delete the loaded effects on exit
	while (it != m_effectList.end())
	{
		Effect* temp = *it;
		it = m_effectList.erase(it);
		delete temp;
		temp = 0;
	}
	SAFE_RELEASE(Vertex::decl);
	SAFE_RELEASE(m_pD3D);
	SAFE_RELEASE(m_pFont);
	SAFE_RELEASE(m_pD3DDevice);
	delete m_pActor;
	m_pActor = 0;
	delete m_pLight;
	m_pLight = 0;
}

//Updates the main app
void NiteShader::Update(float dt)
{
	static float lastPress = 0.f;
	//Set up world matrix
	D3DXMATRIX temp;
	D3DXMatrixScaling(&temp, m_pActor->m_scale.x, m_pActor->m_scale.y, m_pActor->m_scale.z);
	m_world *= temp;
	D3DXMatrixRotationX(&temp, m_pActor->m_rotation.x);
	m_world *= temp;
	D3DXMatrixRotationY(&temp, m_pActor->m_rotation.y);
	m_world *= temp;
	D3DXMatrixRotationZ(&temp, m_pActor->m_rotation.z);
	m_world *= temp;
	D3DXMatrixTranslation(&temp, m_pActor->m_pos.x, m_pActor->m_pos.y, m_pActor->m_pos.z);
	m_world *= temp;
	//Left and right arrows change the effect
	if (g_Input->KeyDown(DIK_LEFT) && lastPress > KEYDELAY)
	{
		m_selectedEffect--;
		if (m_selectedEffect < 0)
			m_selectedEffect = m_effectList.size() - 1;
		lastPress = 0.f;
	}
	else if (g_Input->KeyDown(DIK_RIGHT) && lastPress > KEYDELAY)
	{
		m_selectedEffect++;
		if (m_selectedEffect > m_effectList.size() - 1)
			m_selectedEffect = 0;
		lastPress = 0.f;
	}
	else if (g_Input->KeyDown(DIK_R) && lastPress > KEYDELAY)
	{
		for (int i = 0; i < m_effectList.size(); i++)
		{
			m_effectList[i]->ReCompile();
		}
		lastPress = 0.f;
	}
	else
	{
		lastPress += dt;
	}
	//Update the light
	m_pLight->Update(dt);
	//Update selected effect
	m_effectList[m_selectedEffect]->Update(dt);
}

//Renders a single frame
void NiteShader::Render(float dt)
{
	//Clear the buffers
	HR(m_pD3DDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, m_clearColor, 1.0f, 0));
	//Begin Rendering
	HR(m_pD3DDevice->BeginScene());

	//Render selected effect
	m_effectList[m_selectedEffect]->Render(dt, m_pActor->GetMesh(), m_pActor->GetNumMaterials());

	HR(m_pD3DDevice->EndScene());
	//Present and discard the buffer
	HR(m_pD3DDevice->Present(0, 0, 0, 0));
}

//Cleansup on lost devices
void NiteShader::OnLostDevice()
{
	m_pFont->OnLostDevice();
	//Reset loaded effects
	for(vector<Effect*>::iterator it = m_effectList.begin(); it != m_effectList.end(); it++)
	{
		Effect* temp = *it;
		temp->OnLostDevice();
	}
}

//Resets render states an such on reset devices
void NiteShader::OnResetDevice()
{
	m_pFont->OnResetDevice();
	//Reset Loaded Effects
	for (vector<Effect*>::iterator it = m_effectList.begin(); it != m_effectList.end(); it++)
	{
		Effect* temp = *it;
		temp->OnResetDevice();
	}
	//Reset the camera
	g_Camera->SetProjection(D3DX_PI * 0.25f, m_width/(float)m_height, 1.0f, 1024.0f);
}

//Checks whether a device has been lost
bool NiteShader::IsDeviceLost()
{
	//Check for the device
	HRESULT hr = m_pD3DDevice->TestCooperativeLevel();

	//Device was lost but cannot yet be reset
	if (hr == D3DERR_DEVICELOST)
	{
		Sleep(20);
		return true;
	}
	//Driver error -- exit
	else if (hr == D3DERR_DRIVERINTERNALERROR)
	{
		MessageBox(0, L"Driver internal error...exiting", L"ERROR", MB_OK);
		PostQuitMessage(0);
		return true;
	}
	//Device has been reacquired but not reset yet
	else if (hr == D3DERR_DEVICENOTRESET)
	{
		OnLostDevice();
		HR(m_pD3DDevice->Reset(&m_D3DPP));
		OnResetDevice();
		return false;
	}
	return false;
}


/////////////////////////////////////////////////////////////////HELPER FUNCTIONS///////////////////////////////////////////////////////////////
//Initializes D3D
void NiteShader::InitD3D()
{
		//Acquire pointer to D3D object for enumerating devices
	m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);

	//Get width and height of the window
	RECT rect;
	GetClientRect(g_hWnd, &rect);
	m_width = rect.right - rect.left;
	m_height = rect.bottom - rect.top;

	//Set the presentation parameters
	m_D3DPP.BackBufferWidth = m_width;
	m_D3DPP.BackBufferHeight = m_height;
	m_D3DPP.BackBufferFormat = D3DFMT_UNKNOWN;
	m_D3DPP.BackBufferCount = 1;
	m_D3DPP.MultiSampleType = D3DMULTISAMPLE_NONE;
	m_D3DPP.MultiSampleQuality = 0;
	m_D3DPP.SwapEffect = D3DSWAPEFFECT_DISCARD;
	m_D3DPP.hDeviceWindow = g_hWnd;
	m_D3DPP.Windowed = TRUE;
	m_D3DPP.EnableAutoDepthStencil = TRUE;
	m_D3DPP.AutoDepthStencilFormat = D3DFMT_D24S8;
	m_D3DPP.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
	m_D3DPP.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	m_D3DPP.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	//Get device capabilities
	DWORD deviceBehaviorFlags = 0;
	HR(m_pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &m_D3DCaps));
	//Check the capabilities and set the correct flags
	if (m_D3DCaps.DevCaps & D3DCREATE_HARDWARE_VERTEXPROCESSING)
	{
		deviceBehaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}
	else
	{
		deviceBehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}
	
	if (m_D3DCaps.DevCaps & D3DDEVCAPS_PUREDEVICE && deviceBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING)
	{
		deviceBehaviorFlags |= D3DCREATE_PUREDEVICE;
	}

	//Create the device
	HR(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_hWnd, deviceBehaviorFlags, &m_D3DPP, &m_pD3DDevice));

	//Initialize Vertex Definition
	D3DVERTEXELEMENT9 definition[] = 
	{
		{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
		{0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0},
		{0, 36, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0},
		{0, 48, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};

	HR(m_pD3DDevice->CreateVertexDeclaration(definition, &Vertex::decl));

	//Create the font object
	HR(D3DXCreateFont(m_pD3DDevice, 40, 20, FW_BOLD, 0, false, DEFAULT_CHARSET, 
					OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
					L"Times New Roman", &m_pFont));

	OnResetDevice();
}

//Loads a mesh to display the effects on, if fileName is NULL a sphere is created
void NiteShader::InitActor(char* fileName)
{
	delete m_pActor;
	m_pActor = 0;
	m_pActor = new Actor(this);
	m_pActor->Load(fileName);
}

//Load Effects
void NiteShader::LoadEffects()
{
	//Create effect instances
	Effect* temp = new AmbientEffect(this, "Textures/Grass.bmp");
	m_effectList.push_back(temp);
	temp = new DiffuseEffect(this, "Textures/Grass.bmp");
	m_effectList.push_back(temp);
	temp = new SpecularEffect(this, "Textures/Grass.bmp");
	m_effectList.push_back(temp);
	temp = new GoochEffect(this, D3DXVECTOR3(1.f, 0.f, 0.f), D3DXVECTOR3(0.f, 0.f, 1.f));
	m_effectList.push_back(temp);
	temp = new PhongEffect(this, "Textures/rockwall.jpg");
	m_effectList.push_back(temp);
	temp = new BumpEffect(this, "Textures/rockwall.jpg", "Textures/rockNorm.jpg");
	m_effectList.push_back(temp);
	temp = new ToonEffect(this);
	m_effectList.push_back(temp);
	temp = new SubsurfaceScatterEffect(this, "Textures/SubsurfaceTexture.bmp");
	m_effectList.push_back(temp);
	temp = new ShadowmapEffect(this, "Textures/rockwall.jpg", "Textures/floor.bmp");
	m_effectList.push_back(temp);
	//Initialize effects
	for (int i = 0; i < m_effectList.size(); i++)
	{
		m_effectList[i]->Init();
	}
}

//Prints a string to the screen
void NiteShader::PrintString(char* name, RECT* rect)
{
	HR(m_pFont->DrawTextA(0, name, -1, rect, DT_CENTER | DT_NOCLIP, D3DXCOLOR(1.f, 1.f, 1.f, 1.f)));
}
