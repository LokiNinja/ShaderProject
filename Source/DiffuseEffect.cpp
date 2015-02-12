#include "DiffuseEffect.h"
#include "NiteShader.h"
#include "Globals.h"
#include "Camera.h"

//Constructor
DiffuseEffect::DiffuseEffect(NiteShader* parent, char* texture) : Effect(parent)
{
	m_pFileName = "Shaders/Diffuse.fx";
	m_pEffectName = "DIFFUSE";
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
	//Compile the Shader
	CompileShader();
	if (m_bCompileErrors)
		return;
	{
		string newString(m_pTextureFile);
		wstring tempString(newString.begin(), newString.end());
		HR(D3DXCreateTextureFromFile(m_pParent->GetDevice(), tempString.c_str(), &m_pTexture));
	}
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

//Updates the effect, same for ambient
void DiffuseEffect::Update(float dt)
{
	if (m_bCompileErrors)
		return;
	D3DXMATRIX temp = m_pParent->GetWorld() * g_Camera->GetView() * g_Camera->GetProjection();
	D3DXVECTOR3 lightPos = m_pParent->GetLight()->GetPosition();
	HR(m_pEffect->SetMatrix(m_pWorldViewProjHandle, &temp));
	HR(m_pEffect->SetMatrix(m_pWorldHandle, &m_pParent->GetWorld()));
	HR(m_pEffect->SetValue(m_pLightPositionHandle, lightPos, sizeof(D3DXVECTOR3)));
}

//Renders the effect
void DiffuseEffect::Render(float dt, ID3DXMesh* mesh, int numMaterials)
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
