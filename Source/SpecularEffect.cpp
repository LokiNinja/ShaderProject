#include "SpecularEffect.h"
#include "Globals.h"
#include "Light.h"
#include "ShaderData.h"
#include "Camera.h"

//Constructor
SpecularEffect::SpecularEffect(const WCHAR* texture)
{
	m_pTexture = 0;
	m_pTextureFile = 0;
	m_pWorldViewProjHandle = 0;
	m_pWorldHandle = 0;
	m_pTextureHandle = 0;
	m_pTextureFile = texture;
	m_pLightPositionHandle = 0;
	m_pEyePositionHandle = 0;
	m_pLightColorHandle = 0;
}

//Destructor
SpecularEffect::~SpecularEffect()
{
	Shutdown();
}

//Cleanup
void SpecularEffect::Shutdown()
{
	SAFE_RELEASE(m_pTexture);
}

//Initializes the shader
void SpecularEffect::Init()
{
	HR(D3DXCreateTextureFromFile(m_pDevice, m_pTextureFile, &m_pTexture));

	//Initialize Handles
	m_pWorldViewProjHandle = m_pEffect->GetParameterByName(0, "worldViewProj");
	m_pWorldHandle = m_pEffect->GetParameterByName(0, "world");
	m_pTextureHandle = m_pEffect->GetParameterByName(0, "modelTexture");
	m_pLightPositionHandle = m_pEffect->GetParameterByName(0, "lightPos");
	m_pEyePositionHandle = m_pEffect->GetParameterByName(0, "eyePos");
	m_pLightColorHandle = m_pEffect->GetParameterByName(0, "lightColor");
	//Set the texture
	HR(m_pEffect->SetTexture(m_pTextureHandle, m_pTexture));
	//Set up the technique
	m_pTechniqueHandle = m_pEffect->GetTechniqueByName("Specular");
	HR(m_pEffect->SetTechnique(m_pTechniqueHandle));
}


//Renders the effect
void SpecularEffect::Render(float dt, ID3DXMesh* mesh, int numMaterials)
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

void SpecularEffect::SetData(const ShaderData* data)
{
	D3DXMATRIX WVP = *(data->world) * *(data->view) * *(data->proj);
	D3DXVECTOR3 lightPos = data->light->GetPosition();
	HR(m_pEffect->SetValue(m_pLightColorHandle, data->light->GetSpecular(), sizeof(D3DXVECTOR3)));
	HR(m_pEffect->SetMatrix(m_pWorldViewProjHandle, &WVP));
	HR(m_pEffect->SetMatrix(m_pWorldHandle, data->world));
	HR(m_pEffect->SetValue(m_pLightPositionHandle, lightPos, sizeof(D3DXVECTOR3)));
	HR(m_pEffect->SetValue(m_pEyePositionHandle, data->camera->GetPos(), sizeof(D3DXVECTOR3)));
}

Effect* SpecularEffect::Create(ID3DXBuffer** errors)
{
	SpecularEffect* newEffect = NEW SpecularEffect(L"Textures/Grass.bmp");
	D3DXCreateEffectFromFile(m_pDevice, L"Shaders/Specular.fx", 0, 0, D3DXSHADER_DEBUG, 0, &newEffect->m_pEffect, errors);
	return newEffect;
}
