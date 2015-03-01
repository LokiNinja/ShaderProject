#pragma once
#include "Effect.h"

class ToonEffect : public Effect
{
public:

	~ToonEffect();

	void Init() override;
	void Shutdown() override;
	void SetData(const ShaderData*) override;
	void Render(float dt, ID3DXMesh* mesh, int numMaterials) override;
	static Effect* Create(ID3DXBuffer**);
protected:
	ToonEffect();

	D3DXHANDLE			m_pWorldViewProjHandle;
	D3DXHANDLE			m_pWorldHandle;
	D3DXHANDLE			m_pLightPosHandle;
	D3DXHANDLE			m_pLightDiffuseHandle;
};