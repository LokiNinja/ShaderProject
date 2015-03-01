#pragma once
#include "Effect.h"


class SpecularEffect : public Effect
{
public:
	//Constructor/Destructor

	virtual ~SpecularEffect();

	//Overrides
	void Init() override;
	void Render(float dt, ID3DXMesh* mesh, int numMat) override;
	void SetData(const ShaderData*);
	void Shutdown() override;
	static Effect* Create(ID3DXBuffer**);
protected:
	SpecularEffect(const WCHAR* texture);
	SpecularEffect() {}

	IDirect3DTexture9*			m_pTexture;
	const WCHAR*				m_pTextureFile;

	//Handles
	D3DXHANDLE					m_pWorldHandle;
	D3DXHANDLE					m_pWorldViewProjHandle;
	D3DXHANDLE					m_pTextureHandle;
	D3DXHANDLE					m_pLightPositionHandle;
	D3DXHANDLE					m_pEyePositionHandle;
	D3DXHANDLE					m_pLightColorHandle;
};