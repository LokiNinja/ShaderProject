#include "PhongEffect.h"
#include "Globals.h"
#include "ShaderData.h"
#include "Light.h"
#include "Camera.h"


//Constructor/Destructor
PhongEffect::PhongEffect(const WCHAR* texture)
{
	m_pTextureFile = texture;
	m_pTexture = 0;
	m_pWorldViewProjHandle = 0;
	m_pWorldHandle = 0;
	m_pTextureHandle = 0;
	m_pEyePositionHandle = 0;
	m_pLightPositionHandle = 0;
	m_pLightDiffuseHandle = 0;
	m_pLightSpecularHandle = 0;
	m_pLightAttenuationHandle = 0;
}

PhongEffect::~PhongEffect()
{
	Shutdown();
}

//Interface
void PhongEffect::Init()
{
	HR(D3DXCreateTextureFromFile(m_pDevice, m_pTextureFile, &m_pTexture));
	//Initialize Handles
	m_pWorldViewProjHandle = m_pEffect->GetParameterByName(0, "WorldViewProj");
	m_pWorldHandle = m_pEffect->GetParameterByName(0, "World");
	m_pEyePositionHandle = m_pEffect->GetParameterByName(0, "EyePos");
	m_pLightPositionHandle = m_pEffect->GetParameterByName(0, "LightPos");
	m_pLightDiffuseHandle = m_pEffect->GetParameterByName(0, "LightDif");
	m_pLightSpecularHandle = m_pEffect->GetParameterByName(0, "Specular");
	m_pLightAttenuationHandle = m_pEffect->GetParameterByName(0, "Attenuation");
	m_pTextureHandle = m_pEffect->GetParameterByName(0, "Tex");
	//Set attenuation

	//Set the technique
	m_pTechniqueHandle = m_pEffect->GetTechniqueByName("Phong");
	HR(m_pEffect->SetTechnique(m_pTechniqueHandle));
	//Set the texture
	HR(m_pEffect->SetTexture(m_pTextureHandle, m_pTexture));

}

void PhongEffect::Shutdown()
{
	SAFE_RELEASE(m_pTexture);
}

void PhongEffect::SetData(const ShaderData* data)
{
	D3DXMATRIX WVP = *(data->world) * *(data->view) * *(data->proj);
	D3DXVECTOR3 lightPos = data->light->GetPosition();
	HR(m_pEffect->SetMatrix(m_pWorldViewProjHandle, &WVP));
	HR(m_pEffect->SetMatrix(m_pWorldHandle, data->world));
	HR(m_pEffect->SetValue(m_pEyePositionHandle, data->camera->GetPos(), sizeof(D3DXVECTOR3)));
	HR(m_pEffect->SetValue(m_pLightPositionHandle, lightPos, sizeof(D3DXVECTOR3)));
	HR(m_pEffect->SetValue(m_pLightDiffuseHandle, data->light->GetDiffuse(), sizeof(D3DXVECTOR3)));
	HR(m_pEffect->SetValue(m_pLightSpecularHandle,data->light->GetSpecular(), sizeof(D3DXVECTOR3)));
	HR(m_pEffect->SetValue(m_pLightAttenuationHandle,data->light->GetAttenuation(), sizeof(D3DXVECTOR3)));
}

void PhongEffect::Render(float dt, ID3DXMesh* mesh, int numMaterials)
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

Effect* PhongEffect::Create(ID3DXBuffer** errors)
{
	PhongEffect* newEffect = NEW PhongEffect(L"Textures/rockwall.jpg");
	D3DXCreateEffectFromFile(m_pDevice, L"Shaders/Phong.fx", 0, 0, D3DXSHADER_DEBUG, 0, &newEffect->m_pEffect, errors);
	return newEffect;
}