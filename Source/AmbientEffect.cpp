#include "AmbientEffect.h"
#include "NiteShader.h"
#include "Globals.h"
#include "Camera.h"

//Constructor
AmbientEffect::AmbientEffect( NiteShader* parent, char* texture) : Effect(parent)
{
	m_pFileName = "Shaders/Ambient.fx";
	m_pEffectName = "AMBIENT";
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
	//Compile the effect shader
	CompileShader();
	if (m_bCompileErrors)
		return;
	{
		string newString(m_pTextureFile);
		wstring tempString(newString.begin(), newString.end());
		//Load Texture
		HR(D3DXCreateTextureFromFile(m_pParent->GetDevice(), tempString.c_str(), &m_pTexture));
	}
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

//Updates the effect---this simply sets the matrices each frame
void AmbientEffect::Update(float dt)
{
	if (m_bCompileErrors)
		return;
	//Set matrices
	D3DXMATRIX temp = m_pParent->GetWorld() * g_Camera->GetView() * g_Camera->GetProjection();
	HR(m_pEffect->SetMatrix(m_pWorldViewProjHandle, &temp));
	HR(m_pEffect->SetMatrix(m_pWorldHandle, &m_pParent->GetWorld()));
}

//Render the mesh
void AmbientEffect::Render(float dt, ID3DXMesh* mesh, int numMat)
{
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
	PrintName();
}
