#include "GoochEffect.h"
#include "Globals.h"
#include "NiteShader.h"
#include "Camera.h"

//Constructor
GoochEffect::GoochEffect(NiteShader *parent, D3DXVECTOR3 warmColor, D3DXVECTOR3 coolColor) : Effect(parent)
{
	m_pFileName = "Shaders/Gooch.fx";
	m_pEffectName = "GOOCH";
	m_pWorldViewProjHandle = 0;
	m_pWorldHandle = 0;
	m_pCoolColorHandle = 0;
	m_pWarmColorHandle = 0;
	m_coolColor = coolColor;
	m_warmColor = warmColor;
	m_pLightPositionHandle = 0;
}

//Destructor
GoochEffect::~GoochEffect()
{
	Shutdown();
}

//Shutdown
void GoochEffect::Shutdown()
{
}

//Init
void GoochEffect::Init()
{
	//compile the shader
	CompileShader();
	if (m_bCompileErrors)
		return;
	//Set the handles
	m_pWorldViewProjHandle = m_pEffect->GetParameterByName(0, "worldViewProj");
	m_pWorldHandle = m_pEffect->GetParameterByName(0, "world");
	m_pCoolColorHandle = m_pEffect->GetParameterByName(0, "coolColor");
	m_pWarmColorHandle = m_pEffect->GetParameterByName(0, "warmColor");
	m_pLightPositionHandle = m_pEffect->GetParameterByName(0, "lightPos");
	//Set the warm and cool colors
	HR(m_pEffect->SetValue(m_pCoolColorHandle, m_coolColor, sizeof(D3DXVECTOR3)));
	HR(m_pEffect->SetValue(m_pWarmColorHandle, m_warmColor, sizeof(D3DXVECTOR3)));
	//Set the technique
	m_pTechniqueHandle = m_pEffect->GetTechniqueByName("Gooch");
	HR(m_pEffect->SetTechnique(m_pTechniqueHandle));
}

//Update
void GoochEffect::Update(float dt)
{
	if (m_bCompileErrors)
		return;
	//Just set the matrices
	D3DXMATRIX temp = m_pParent->GetWorld() * g_Camera->GetView() * g_Camera->GetProjection();
	D3DXVECTOR3 lightPos = m_pParent->GetLight()->GetPosition();
	HR(m_pEffect->SetMatrix(m_pWorldViewProjHandle, &temp));
	HR(m_pEffect->SetMatrix(m_pWorldHandle, &m_pParent->GetWorld()));
	HR(m_pEffect->SetValue(m_pLightPositionHandle, lightPos, sizeof(D3DXVECTOR3)));
	PrintName();
}

//Render
void GoochEffect::Render(float dt, ID3DXMesh* mesh, int numMaterials)
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