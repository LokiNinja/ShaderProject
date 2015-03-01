#include "DefaultEffect.h"
#include "Globals.h"
#include "ShaderData.h"


void DefaultEffect::Init()
{
	m_pWorldViewProjHandle = m_pEffect->GetParameterByName(0, "worldViewProj");
	m_pTechniqueHandle = m_pEffect->GetTechniqueByName("Default");
	HR(m_pEffect->SetTechnique(m_pTechniqueHandle));
}

void DefaultEffect::Render(float dt, ID3DXMesh* mesh, int numMat)
{
	HR(m_pDevice->BeginScene());
	UINT passes;
	HR(m_pEffect->Begin(&passes, 0));
	for (UINT i = 0; i < passes; i++)
	{
		HR(m_pEffect->BeginPass(i));
		for (int j = 0; j < numMat; j++)
			HR(mesh->DrawSubset(j));
		HR(m_pEffect->EndPass());
	}
	HR(m_pEffect->End());
	HR(m_pDevice->EndScene());
}

Effect* DefaultEffect::Create(ID3DXBuffer** m_pErrors)
{
	DefaultEffect* newEffect = NEW DefaultEffect();
	D3DXCreateEffectFromFile(m_pDevice, L"Shaders/Default.fx", 0, 0, D3DXSHADER_DEBUG, 0, &newEffect->m_pEffect, m_pErrors);
	return newEffect;
}

void DefaultEffect::SetData(const ShaderData* data)
{
	D3DXMATRIX temp = *(data->world) * *(data->view) * *(data->proj);
	HR(m_pEffect->SetMatrix(m_pWorldViewProjHandle, &temp));
}

