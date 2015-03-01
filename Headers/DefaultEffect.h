#pragma once
#include "Effect.h"

class DefaultEffect : public Effect
{
public:
	void Init() override;
	void Render(float dt, ID3DXMesh* mesh, int numMat);
	void SetData(const ShaderData*) override;
	static Effect* Create(ID3DXBuffer**);
private:
	D3DXHANDLE					m_pWorldViewProjHandle;

};