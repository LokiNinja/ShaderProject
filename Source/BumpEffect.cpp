#include "BumpEffect.h"
#include "Camera.h"
#include "Globals.h"
#include "NiteShader.h"

//Constructor
BumpEffect::BumpEffect(NiteShader* parent, char* texture, char* normal) : Effect(parent)
{
	m_pTextureFile = texture;
	m_pNormalMapFile = normal;
	m_pTexture = 0;
	m_pNormalMap = 0;
	m_pFileName = "Shaders/Bump.fx";
	m_pEffectName = "BUMP MAP";

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
	//Compile shader
	CompileShader();
	if (m_bCompileErrors)
		return;

	//Load texture and normal map
	{
		string filename(m_pTextureFile);
		wstring newString(filename.begin(), filename.end());
		HR(D3DXCreateTextureFromFile(m_pParent->GetDevice(), newString.c_str(), &m_pTexture));
	}
	{
		string filename(m_pNormalMapFile);
		wstring newString(filename.begin(), filename.end());
		HR(D3DXCreateTextureFromFile(m_pParent->GetDevice(), newString.c_str() , &m_pNormalMap));
	}

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

	//Set light properties
	m_pParent->GetLight()->SetAttenuation(D3DXVECTOR3(0.f, .4f, 0.f));
	HR(m_pEffect->SetValue(m_pLightDiffuseHandle, m_pParent->GetLight()->GetDiffuse(), sizeof(D3DXVECTOR3)));
	HR(m_pEffect->SetValue(m_pLightAttenuationHandle, m_pParent->GetLight()->GetAttenuation(), sizeof(D3DXVECTOR3)));
}

void BumpEffect::Update(float dt)
{
	if (m_bCompileErrors)
		return;

	D3DXMATRIX temp = m_pParent->GetWorld() * g_Camera->GetViewProj();
	D3DXVECTOR3 lightPos = m_pParent->GetLight()->GetPosition();
	HR(m_pEffect->SetMatrix(m_pWorldViewProjHandle, &temp));
	HR(m_pEffect->SetMatrix(m_pWorldHandle, &m_pParent->GetWorld()));
	D3DXMatrixInverse(&temp, 0, &temp);
	HR(m_pEffect->SetMatrix(m_pWorldInvHandle, &temp));
	HR(m_pEffect->SetValue(m_pEyePosHandle, g_Camera->GetPos(), sizeof(D3DXVECTOR3)));
	HR(m_pEffect->SetValue(m_pLightDirHandle, lightPos, sizeof(D3DXVECTOR3)));
}

void BumpEffect::Render(float dt, ID3DXMesh* mesh, int numMaterials)
{
	if (m_bCompileErrors)
		return;

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
