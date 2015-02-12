#include "SpecularEffect.h"
#include "Globals.h"
#include "NiteShader.h"
#include "Camera.h"

//Constructor
SpecularEffect::SpecularEffect(NiteShader* parent, char* texture) : Effect(parent)
{
	m_pTexture = 0;
	m_pTextureFile = 0;
	m_pWorldViewProjHandle = 0;
	m_pWorldHandle = 0;
	m_pTextureHandle = 0;
	m_pFileName = "Shaders/Specular.fx";
	m_pEffectName = "SPECULAR";
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
	//Compile the shader
	CompileShader();
	if (m_bCompileErrors)
	{
		Shutdown();
		return;
	}
	{
		string newString(m_pTextureFile);
		wstring tempString(newString.begin(), newString.end());
		HR(D3DXCreateTextureFromFile(m_pParent->GetDevice(), tempString.c_str(), &m_pTexture));
	}

	//Initialize Handles
	m_pWorldViewProjHandle = m_pEffect->GetParameterByName(0, "worldViewProj");
	m_pWorldHandle = m_pEffect->GetParameterByName(0, "world");
	m_pTextureHandle = m_pEffect->GetParameterByName(0, "modelTexture");
	m_pLightPositionHandle = m_pEffect->GetParameterByName(0, "lightPos");
	m_pEyePositionHandle = m_pEffect->GetParameterByName(0, "eyePos");
	m_pLightColorHandle = m_pEffect->GetParameterByName(0, "lightColor");
	//Set the texture
	HR(m_pEffect->SetTexture(m_pTextureHandle, m_pTexture));
	//Set the light color
	HR(m_pEffect->SetValue(m_pLightColorHandle, m_pParent->GetLight()->GetSpecular(), sizeof(D3DXVECTOR3)));
	//Set up the technique
	m_pTechniqueHandle = m_pEffect->GetTechniqueByName("Specular");
	HR(m_pEffect->SetTechnique(m_pTechniqueHandle));
}


//Renders the effect
void SpecularEffect::Render(float dt, ID3DXMesh* mesh, int numMaterials)
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

//Updates the shader
void SpecularEffect::Update(float dt)
{
	if (m_bCompileErrors)
		return;
	D3DXMATRIX temp = m_pParent->GetWorld() * g_Camera->GetView() * g_Camera->GetProjection();
	D3DXVECTOR3 lightPos = m_pParent->GetLight()->GetPosition();
	HR(m_pEffect->SetMatrix(m_pWorldViewProjHandle, &temp));
	HR(m_pEffect->SetMatrix(m_pWorldHandle, &m_pParent->GetWorld()));
	HR(m_pEffect->SetValue(m_pLightPositionHandle, lightPos, sizeof(D3DXVECTOR3)));
	HR(m_pEffect->SetValue(m_pEyePositionHandle, g_Camera->GetPos(), sizeof(D3DXVECTOR3)));
}