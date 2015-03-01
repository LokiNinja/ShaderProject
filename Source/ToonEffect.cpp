#include "ShaderData.h"
#include "Light.h"
#include "Globals.h"
#include "Camera.h"
#include "ToonEffect.h"

ToonEffect::ToonEffect()
{

	m_pWorldViewProjHandle = 0;
	m_pWorldHandle = 0;
	m_pLightPosHandle = 0;
	m_pLightDiffuseHandle = 0;
}

ToonEffect::~ToonEffect()
{
	Shutdown();
}

void ToonEffect::Shutdown()
{
}

void ToonEffect::Init()
{
	//Get and set technique
	m_pTechniqueHandle = m_pEffect->GetTechniqueByName("Toon");
	HR(m_pEffect->SetTechnique(m_pTechniqueHandle));

	//Get handles
	m_pWorldViewProjHandle = m_pEffect->GetParameterByName(0, "worldViewProj");
	m_pWorldHandle = m_pEffect->GetParameterByName(0, "world");
	m_pLightPosHandle = m_pEffect->GetParameterByName(0, "lightPos");
	m_pLightDiffuseHandle = m_pEffect->GetParameterByName(0, "lightDif");

}

void ToonEffect::SetData(const ShaderData* data)
{
	D3DXMATRIX WVP = *(data->world) * *(data->view) * *(data->proj);
	HR(m_pEffect->SetMatrix(m_pWorldHandle, data->world));
	HR(m_pEffect->SetMatrix(m_pWorldViewProjHandle, &WVP));
	HR(m_pEffect->SetValue(m_pLightPosHandle, data->light->GetPosition(), sizeof(D3DXVECTOR3)));
	HR(m_pEffect->SetValue(m_pLightDiffuseHandle, data->light->GetDiffuse(), sizeof(D3DXVECTOR3)));
}

void ToonEffect::Render(float dt, ID3DXMesh* mesh, int numMaterials)
{
	HR(m_pDevice->BeginScene());
	UINT passes;

	HR(m_pEffect->Begin(&passes, 0));
	for (UINT i = 0; i < passes; i++)
	{
		HR(m_pEffect->BeginPass(i));
		for (int j = 0; j < numMaterials; j++)
			HR(mesh->DrawSubset(j));
		HR(m_pEffect->EndPass());
	}
	HR(m_pEffect->End());
	HR(m_pDevice->EndScene());
}

Effect* ToonEffect::Create(ID3DXBuffer** errors)
{
	ToonEffect* newEffect = NEW ToonEffect();
	D3DXCreateEffectFromFile(m_pDevice, L"Shaders/Toon.fx", 0, 0, D3DXSHADER_DEBUG, 0, &newEffect->m_pEffect, errors);
	return newEffect;
}