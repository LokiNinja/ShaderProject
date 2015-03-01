#include "BumpEffect.h"
#include "Camera.h"
#include "Globals.h"
#include "Light.h"
#include "ShaderData.h"

//Constructor
BumpEffect::BumpEffect(const WCHAR* texture, const WCHAR* normal)
{
	m_pTextureFile = texture;
	m_pNormalMapFile = normal;
	m_pTexture = 0;
	m_pNormalMap = 0;

	//Handles
	m_pWorldViewProjHandle = 0;
	m_pWorldInvHandle = 0;
	m_pLightDirHandle = 0;
	m_pEyePosHandle = 0;
	m_pLightDiffuseHandle = 0;
	m_pLightAttenuationHandle = 0;
	m_pTextureHandle = 0;
	m_pNormalMapHandle = 0;
	m_pWorldHandle = 0;
}

//Destructor
BumpEffect::~BumpEffect()
{
	Shutdown();
}

//Shutdown
void BumpEffect::Shutdown()
{
	SAFE_RELEASE(m_pTexture);
	SAFE_RELEASE(m_pNormalMap);
}

//Initialize
void BumpEffect::Init()
{
	HR(D3DXCreateTextureFromFile(m_pDevice, m_pTextureFile, &m_pTexture));
	HR(D3DXCreateTextureFromFile(m_pDevice, m_pNormalMapFile , &m_pNormalMap));

	//Set the technique
	m_pTechniqueHandle = m_pEffect->GetTechniqueByName("Bump");
	HR(m_pEffect->SetTechnique(m_pTechniqueHandle));

	//Get handles
	m_pWorldViewProjHandle = m_pEffect->GetParameterByName(0, "worldViewProj");
	m_pWorldInvHandle = m_pEffect->GetParameterByName(0, "worldInv");
	m_pLightDirHandle = m_pEffect->GetParameterByName(0, "lightPos");
	m_pEyePosHandle = m_pEffect->GetParameterByName(0, "eyePosW");
	m_pTextureHandle = m_pEffect->GetParameterByName(0, "tex");
	m_pNormalMapHandle = m_pEffect->GetParameterByName(0, "normalMap");
	m_pWorldHandle = m_pEffect->GetParameterByName(0, "world");
	m_pLightDiffuseHandle = m_pEffect->GetParameterByName(0, "lightDif");
	m_pLightAttenuationHandle = m_pEffect->GetParameterByName(0, "attenuation");

	//Set texture handles
	HR(m_pEffect->SetTexture(m_pTextureHandle, m_pTexture));
	HR(m_pEffect->SetTexture(m_pNormalMapHandle, m_pNormalMap));
}

void BumpEffect::SetData(const ShaderData* data)
{
	D3DXMATRIX worldView = *(data->world) * *(data->view);
	D3DXMATRIX worldInv;
	D3DXMatrixInverse(&worldInv, 0, data->world);
	D3DXMATRIX WVP = worldView * *(data->proj);
	HR(m_pEffect->SetValue(m_pLightAttenuationHandle, data->light->GetAttenuation(), sizeof(D3DXVECTOR3)));
	HR(m_pEffect->SetValue(m_pLightDiffuseHandle, data->light->GetDiffuse(), sizeof(D3DXVECTOR3)));
	HR(m_pEffect->SetMatrix(m_pWorldViewProjHandle, &WVP));
	HR(m_pEffect->SetMatrix(m_pWorldHandle, data->world));
	HR(m_pEffect->SetMatrix(m_pWorldInvHandle, &worldInv));
	HR(m_pEffect->SetValue(m_pLightDirHandle, data->light->GetPosition(), sizeof(D3DXVECTOR3)));
	HR(m_pEffect->SetValue(m_pEyePosHandle, data->camera->GetPos(), sizeof(D3DXVECTOR3)));
}

void BumpEffect::Render(float dt, ID3DXMesh* mesh, int numMaterials)
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

Effect* BumpEffect::Create(ID3DXBuffer** errors)
{
	BumpEffect* newEffect = NEW BumpEffect(L"Textures/rockwall.jpg", L"Textures/rockNorm.jpg");
	D3DXCreateEffectFromFile(m_pDevice, L"Shaders/Bump.fx", 0, 0, D3DXSHADER_DEBUG, 0, &newEffect->m_pEffect, errors);
	return newEffect;
}
