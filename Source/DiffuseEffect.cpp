#include "DiffuseEffect.h"
#include "NiteShader.h"
#include "Globals.h"
#include "ShaderData.h"

//Constructor
DiffuseEffect::DiffuseEffect(const WCHAR* texture)
{
	m_pTexture = 0;
	m_pTextureFile = texture;
	m_pWorldViewProjHandle = 0;
	m_pWorldHandle = 0;
	m_pTextureHandle = 0;
	m_pLightPositionHandle = 0;
}

//Destructor
DiffuseEffect::~DiffuseEffect()
{
	Shutdown();
}

//Performs cleanup
void DiffuseEffect::Shutdown()
{
	SAFE_RELEASE(m_pTexture);
}

//Initializes the shader
void DiffuseEffect::Init()
{
	HR(D3DXCreateTextureFromFile(m_pDevice, m_pTextureFile, &m_pTexture));
	//Initialize handles
	m_pWorldViewProjHandle = m_pEffect->GetParameterByName(0, "worldViewProj");
	m_pWorldHandle = m_pEffect->GetParameterByName(0, "world");
	m_pTextureHandle = m_pEffect->GetParameterByName(0, "modelTexture");
	m_pLightPositionHandle = m_pEffect->GetParameterByName(0, "lightPos");
	//Set the technique
	m_pTechniqueHandle = m_pEffect->GetTechniqueByName("Diffuse");
	HR(m_pEffect->SetTechnique(m_pTechniqueHandle));
	//Set the texture
	HR(m_pEffect->SetTexture(m_pTextureHandle, m_pTexture));
}

void DiffuseEffect::SetData(const ShaderData* data)
{
	D3DXMATRIX temp = *(data->world) * *(data->view) * *(data->proj);
	D3DXVECTOR3 lightPos = data->light->GetPosition();
	HR(m_pEffect->SetMatrix(m_pWorldViewProjHandle, &temp));
	HR(m_pEffect->SetMatrix(m_pWorldHandle, data->world));
	HR(m_pEffect->SetValue(m_pLightPositionHandle, lightPos, sizeof(D3DXVECTOR3)));
}

//Renders the effect
void DiffuseEffect::Render(float dt, ID3DXMesh* mesh, int numMaterials)
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

Effect* DiffuseEffect::Create(ID3DXBuffer** errors)
{
	DiffuseEffect* newEffect = NEW DiffuseEffect(L"Textures/Grass.bmp");
	D3DXCreateEffectFromFile(m_pDevice, L"Shaders/Diffuse.fx", 0, 0, D3DXSHADER_DEBUG, 0, &newEffect->m_pEffect, errors);
	return newEffect;
}
