#include "PhongEffect.h"
#include "Globals.h"
#include "NiteShader.h"
#include "Camera.h"


//Constructor/Destructor
PhongEffect::PhongEffect(NiteShader* parent, char* texture) : Effect(parent)
{
	m_pTextureFile = texture;
	m_pTexture = 0;
	m_pFileName = "Shaders/Phong.fx";
	m_pEffectName = "PHONG";
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
	//Compile the shader
	CompileShader();
	if (m_bCompileErrors)
		return;
	{
		string newString(m_pTextureFile);
		wstring tempString(newString.begin(), newString.end());
		HR(D3DXCreateTextureFromFile(m_pParent->GetDevice(), tempString.c_str(), &m_pTexture));
	}
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
	m_pParent->GetLight()->SetAttenuation(D3DXVECTOR3(0.f, .5f, 0.f));
	//Set the technique
	m_pTechniqueHandle = m_pEffect->GetTechniqueByName("Phong");
	HR(m_pEffect->SetTechnique(m_pTechniqueHandle));
	//Set the texture
	HR(m_pEffect->SetTexture(m_pTextureHandle, m_pTexture));
	//and other constants
	HR(m_pEffect->SetValue(m_pLightDiffuseHandle, m_pParent->GetLight()->GetDiffuse(), sizeof(D3DXVECTOR3)));
	HR(m_pEffect->SetValue(m_pLightSpecularHandle, m_pParent->GetLight()->GetSpecular(), sizeof(D3DXVECTOR3)));
	HR(m_pEffect->SetValue(m_pLightAttenuationHandle, m_pParent->GetLight()->GetAttenuation(), sizeof(D3DXVECTOR3)));
}

void PhongEffect::Shutdown()
{
	SAFE_RELEASE(m_pTexture);
}

void PhongEffect::Update(float dt)
{
	if (m_bCompileErrors)
		return;
	D3DXMATRIX temp = m_pParent->GetWorld() * g_Camera->GetView() * g_Camera->GetProjection();
	D3DXVECTOR3 lightPos = m_pParent->GetLight()->GetPosition();
	HR(m_pEffect->SetMatrix(m_pWorldViewProjHandle, &temp));
	HR(m_pEffect->SetMatrix(m_pWorldHandle, &m_pParent->GetWorld()));
	HR(m_pEffect->SetValue(m_pEyePositionHandle, g_Camera->GetPos(), sizeof(D3DXVECTOR3)));
	HR(m_pEffect->SetValue(m_pLightPositionHandle, lightPos, sizeof(D3DXVECTOR3)));
}

void PhongEffect::Render(float dt, ID3DXMesh* mesh, int numMaterials)
{
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
	PrintName();
}