#include "AmbientEffect.h"
#include "Globals.h"
#include "ShaderData.h"

//Constructor
AmbientEffect::AmbientEffect(const WCHAR* texture)
{
	m_pTexture = 0;
	m_pTextureFile = texture;
	m_pWorldViewProjHandle = 0;
	m_pWorldHandle = 0;
	m_pTextureHandle = 0;
}

//Destructor
AmbientEffect::~AmbientEffect()
{
	Shutdown();
}

//Initialize handles and such
void AmbientEffect::Init()
{
	//Load Texture
	HR(D3DXCreateTextureFromFile(m_pDevice, m_pTextureFile, &m_pTexture));
	//Initialize handles
	m_pWorldViewProjHandle = m_pEffect->GetParameterByName(0, "worldViewProj");
	m_pWorldHandle = m_pEffect->GetParameterByName(0, "world");
	m_pTextureHandle = m_pEffect->GetParameterByName(0, "modelTexture");
	//Set the technique
	m_pTechniqueHandle = m_pEffect->GetTechniqueByName("Ambient");
	HR(m_pEffect->SetTechnique(m_pTechniqueHandle));
	//Texture doesn't change so set it up here
	HR(m_pEffect->SetTexture(m_pTextureHandle, m_pTexture));
}

//Free assets
void AmbientEffect::Shutdown()
{
	SAFE_RELEASE(m_pTexture);
}

void AmbientEffect::SetData(const ShaderData* data)
{
	D3DXMATRIX temp = *(data->world) * *(data->view) * *(data->proj);
	HR(m_pEffect->SetMatrix(m_pWorldViewProjHandle, &temp));
	HR(m_pEffect->SetMatrix(m_pWorldHandle, data->world));
}

//Render the mesh
void AmbientEffect::Render(float dt, ID3DXMesh* mesh, int numMat)
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

Effect* AmbientEffect::Create(ID3DXBuffer** errors)
{
	AmbientEffect* newEffect = NEW AmbientEffect(L"Textures/Grass.bmp");
	D3DXCreateEffectFromFile(m_pDevice, L"Shaders/Ambient.fx", 0, 0, D3DXSHADER_DEBUG, 0, &newEffect->m_pEffect, errors);
	return newEffect;
}
