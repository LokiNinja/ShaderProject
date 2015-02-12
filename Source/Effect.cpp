#include "Effect.h"
#include "Globals.h"
#include "NiteShader.h"

//Constructor
Effect::Effect(NiteShader* parent)
{
	m_pEffect = 0;
	m_pFileName = 0;
	m_pEffectName = 0;
	m_pErrors = 0;
	m_pTechniqueHandle = 0;
	m_pParent = parent;
	CompileDefault();
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
	SAFE_RELEASE(m_pErrors);
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

void Effect::CompileShader()
{
	Effect::Shutdown();
	//Convert the c string to wchar
	{
		string filename(m_pFileName);
		wstring tempString(filename.begin(), filename.end());
		D3DXCreateEffectFromFile(m_pParent->GetDevice(), tempString.c_str(), 0, 0, D3DXSHADER_DEBUG, 0, &m_pEffect, &m_pErrors);
	}
	if (m_pErrors)
	{
		string filename(m_pFileName);
		wstring tempString(filename.begin(), filename.end());
		MessageBox(0, tempString.c_str(), L"ERROR", MB_OK);
		m_bCompileErrors = true;
		CompileDefault();
	}
	else
	{
		m_bCompileErrors = false;
	}
}

//Prints the effect name to the screen
void Effect::PrintName()
{
	RECT rect = { m_pParent->GetWidth(), 10, 0, 0};
	m_pParent->PrintString(m_pEffectName, &rect);
}

//Compiles the default shader so that runtime typos or errors
//dont ruin the seamless debugging experience
void Effect::CompileDefault()
{
	Effect::Shutdown();
	//Compile
	D3DXCreateEffectFromFile(m_pParent->GetDevice(), L"Shaders/Default.fx", 0, 0, D3DXSHADER_DEBUG, 0, &m_pEffect, &m_pErrors);
	if (m_pErrors)
	{
		string newString((char*)m_pErrors->GetBufferPointer());
		wstring tempString(newString.begin(), newString.end());
		MessageBox(0, tempString.c_str(), L"ERROR", MB_OK);
	}
	m_pTechniqueHandle = m_pEffect->GetTechniqueByName("Default");
	HR(m_pEffect->SetTechnique(m_pTechniqueHandle));
}

//Recompiles the shader during runtime
void Effect::ReCompile()
{
	Shutdown();
	Effect::Shutdown();
	Init();
}
