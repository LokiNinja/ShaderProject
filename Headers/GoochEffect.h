#pragma once
#include "Effect.h"

class GoochEffect : public Effect
{
public:
	//constructor/destructor

	virtual ~GoochEffect();

	//Effect Interface
	void Init() override;
	void Shutdown();
	void Render(float dt, ID3DXMesh* mesh, int numMaterials) override;
	void SetData(const ShaderData*) override;
	static Effect* Create(ID3DXBuffer**);
private:
	GoochEffect(D3DXVECTOR3 warmColor, D3DXVECTOR3 coolColor);
	GoochEffect() {}

	//Colors
	D3DXVECTOR3 m_warmColor;
	D3DXVECTOR3 m_coolColor;
	//Shader handles
	D3DXHANDLE m_pWorldViewProjHandle;
	D3DXHANDLE m_pWorldHandle;
	D3DXHANDLE m_pWarmColorHandle;
	D3DXHANDLE m_pCoolColorHandle;
	D3DXHANDLE m_pLightPositionHandle;
};