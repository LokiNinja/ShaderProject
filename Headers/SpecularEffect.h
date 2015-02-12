#pragma once
#include "Effect.h"


class SpecularEffect : public Effect
{
public:
	//Constructor/Destructor
	SpecularEffect(NiteShader* parent, char* texture);
	virtual ~SpecularEffect();

	//Overrides
	void Init() override;
	void Render(float dt, ID3DXMesh* mesh, int numMat) override;
	void Update(float dt) override;
	void Shutdown() override;
protected:
	IDirect3DTexture9*			m_pTexture;
	char*						m_pTextureFile;

	//Handles
	D3DXHANDLE					m_pWorldHandle;
	D3DXHANDLE					m_pWorldViewProjHandle;
	D3DXHANDLE					m_pTextureHandle;
	D3DXHANDLE					m_pLightPositionHandle;
	D3DXHANDLE					m_pEyePositionHandle;
	D3DXHANDLE					m_pLightColorHandle;
};