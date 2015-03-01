#pragma once
#include "Effect.h"

class DiffuseEffect : public Effect
{
public:
	//Constructor Destructor

	virtual ~DiffuseEffect();

	//Base class overrides
	void Init() override;
	void Render(float dt, ID3DXMesh* mesh, int numMat) override;
	void Shutdown() override;
	void SetData(const ShaderData*) override;
	static Effect* Create(ID3DXBuffer**);
private:
	DiffuseEffect(const WCHAR* texture);
	DiffuseEffect() {}

	//Texture info
	IDirect3DTexture9*			m_pTexture;
	const WCHAR*				m_pTextureFile;
	//Handles
	D3DXHANDLE					m_pWorldViewProjHandle;
	D3DXHANDLE					m_pWorldHandle;
	D3DXHANDLE					m_pTextureHandle;
	D3DXHANDLE					m_pLightPositionHandle;
};