#include "NiteShader.h"
#include "Globals.h"
#include "Camera.h"
#include "ToonEffect.h"

ToonEffect::ToonEffect(NiteShader* parent) : Effect(parent)
{

	m_pWorldViewProjHandle = 0;
	m_pWorldHandle = 0;
	m_pLightPosHandle = 0;
	m_pLightDiffuseHandle = 0;

	m_pFileName = "Shaders/Toon.fx";
	m_pEffectName = "TOON";
}

ToonEffect::~ToonEffect()
{
	Shutdown();
}

void ToonEffect::Shutdown()
{
}

void ToonEffect::Init()
{
	CompileShader();

	if (m_bCompileErrors)
		return;

	//Get and set technique
	m_pTechniqueHandle = m_pEffect->GetTechniqueByName("Toon");
	HR(m_pEffect->SetTechnique(m_pTechniqueHandle));

	//Get handles
	m_pWorldViewProjHandle = m_pEffect->GetParameterByName(0, "worldViewProj");
	m_pWorldHandle = m_pEffect->GetParameterByName(0, "world");
	m_pLightPosHandle = m_pEffect->GetParameterByName(0, "lightPos");
	m_pLightDiffuseHandle = m_pEffect->GetParameterByName(0, "lightDif");

	//Set constant handles
	HR(m_pEffect->SetValue(m_pLightDiffuseHandle, &m_pParent->GetLight()->GetDiffuse(), sizeof(D3DXVECTOR3)));
}

void ToonEffect::Update(float dt)
{
	if (m_bCompileErrors)
		return;

	D3DXMATRIX temp = m_pParent->GetWorld() * g_Camera->GetViewProj();
	HR(m_pEffect->SetMatrix(m_pWorldHandle, &m_pParent->GetWorld()));
	HR(m_pEffect->SetMatrix(m_pWorldViewProjHandle, &temp));
	HR(m_pEffect->SetValue(m_pLightPosHandle, m_pParent->GetLight()->GetPosition(), sizeof(D3DXVECTOR3)));
}

void ToonEffect::Render(float dt, ID3DXMesh* mesh, int numMaterials)
{
	if (m_bCompileErrors)
		return;

	UINT passes;

	HR(m_pEffect->Begin(&passes, 0));
	for (UINT i = 0; i < passes; i++)
	{
		HR(m_pEffect->BeginPass(i));
		for (int j = 0; j < numMaterials; j++)
			HR(mesh->DrawSubset(j));
		HR(m_pEffect->EndPass());
	}
	HR(m_pEffect->End());
	PrintName();
}