#pragma once
#include "Effect.h"

class ToonEffect : public Effect
{
public:
	ToonEffect(NiteShader* parent);
	~ToonEffect();

	void Init() override;
	void Shutdown() override;
	void Update(float dt) override;
	void Render(float dt, ID3DXMesh* mesh, int numMaterials) override;
protected:

	D3DXHANDLE			m_pWorldViewProjHandle;
	D3DXHANDLE			m_pWorldHandle;
	D3DXHANDLE			m_pLightPosHandle;
	D3DXHANDLE			m_pLightDiffuseHandle;
};