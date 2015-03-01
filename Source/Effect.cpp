#include "Effect.h"
#include "Globals.h"
#include "NiteShader.h"

IDirect3DDevice9* Effect::m_pDevice = nullptr;

//Constructor
Effect::Effect()
{
	m_pEffect = 0;
	m_pTechniqueHandle = 0;
}

//Destructor
Effect::~Effect()
{
	Shutdown();
}

//Shuts down
void Effect::Shutdown()
{
	SAFE_RELEASE(m_pEffect);
}

//Given
void Effect::OnLostDevice()
{
	m_pEffect->OnLostDevice();
}

//Given
void Effect::OnResetDevice()
{
	m_pEffect->OnResetDevice();
}

void Effect::SetDevice(IDirect3DDevice9* dev)
{
	if (m_pDevice == nullptr)
		m_pDevice = dev;
}