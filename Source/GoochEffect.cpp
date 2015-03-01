#include "GoochEffect.h"
#include "Globals.h"
#include "ShaderData.h"
#include "Light.h"
#include "Camera.h"

//Constructor
GoochEffect::GoochEffect( D3DXVECTOR3 warmColor, D3DXVECTOR3 coolColor)
{
	m_pWorldViewProjHandle = 0;
	m_pWorldHandle = 0;
	m_pCoolColorHandle = 0;
	m_pWarmColorHandle = 0;
	m_coolColor = coolColor;
	m_warmColor = warmColor;
	m_pLightPositionHandle = 0;
}

//Destructor
GoochEffect::~GoochEffect()
{
	Shutdown();
}

//Shutdown
void GoochEffect::Shutdown()
{
}

//Init
void GoochEffect::Init()
{
	//Set the handles
	m_pWorldViewProjHandle = m_pEffect->GetParameterByName(0, "worldViewProj");
	m_pWorldHandle = m_pEffect->GetParameterByName(0, "world");
	m_pCoolColorHandle = m_pEffect->GetParameterByName(0, "coolColor");
	m_pWarmColorHandle = m_pEffect->GetParameterByName(0, "warmColor");
	m_pLightPositionHandle = m_pEffect->GetParameterByName(0, "lightPos");
	//Set the warm and cool colors
	HR(m_pEffect->SetValue(m_pCoolColorHandle, m_coolColor, sizeof(D3DXVECTOR3)));
	HR(m_pEffect->SetValue(m_pWarmColorHandle, m_warmColor, sizeof(D3DXVECTOR3)));
	//Set the technique
	m_pTechniqueHandle = m_pEffect->GetTechniqueByName("Gooch");
	HR(m_pEffect->SetTechnique(m_pTechniqueHandle));
}

void GoochEffect::SetData(const ShaderData* data)
{
	//Just set the matrices
	D3DXMATRIX WVP = *(data->world) * *(data->view) * *(data->proj);
	D3DXVECTOR3 lightPos = data->light->GetPosition();
	HR(m_pEffect->SetMatrix(m_pWorldViewProjHandle, &WVP));
	HR(m_pEffect->SetMatrix(m_pWorldHandle, data->world));
	HR(m_pEffect->SetValue(m_pLightPositionHandle, lightPos, sizeof(D3DXVECTOR3)));
}

//Render
void GoochEffect::Render(float dt, ID3DXMesh* mesh, int numMaterials)
{
	HR(m_pDevice->BeginScene());
	UINT passes;
	HR(m_pEffect->Begin(&passes, 0));
	for (UINT i = 0; i < passes; i++)
	{
		HR(m_pEffect->BeginPass(i));
		for (int j = 0; j < numMaterials; j++)
		{
			HR(mesh->DrawSubset(j));
		}
		HR(m_pEffect->EndPass());
	}
	HR(m_pEffect->End());
	HR(m_pDevice->EndScene());
}

Effect* GoochEffect::Create(ID3DXBuffer** errors)
{
	GoochEffect* newEffect = NEW GoochEffect(D3DXVECTOR3(1.f, 0.f, 0.f), D3DXVECTOR3(0.f, 0.f, 1.f));
	D3DXCreateEffectFromFile(m_pDevice, L"Shaders/Gooch.fx", 0, 0, D3DXSHADER_DEBUG, 0, &newEffect->m_pEffect, errors);
	return newEffect;
}