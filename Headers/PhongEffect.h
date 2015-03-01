#pragma once
#include "Effect.h"

class PhongEffect : public Effect
{
public:
	//Constructor/Destructor

	virtual ~PhongEffect();

	//Interface
	void Init() override;
	void Shutdown() override;
	void Render(float dt, ID3DXMesh* mesh, int numMaterials) override;
	void SetData(const ShaderData*) override;
	static Effect* Create(ID3DXBuffer**);
protected:
	PhongEffect(const WCHAR* texture);
	PhongEffect() {}

	//Texture stuff
	IDirect3DTexture9*		m_pTexture;
	const WCHAR*			m_pTextureFile;

	//Handles
	D3DXHANDLE				m_pWorldViewProjHandle;
	D3DXHANDLE				m_pWorldHandle;
	D3DXHANDLE				m_pTextureHandle;
	D3DXHANDLE				m_pEyePositionHandle;
	D3DXHANDLE				m_pLightPositionHandle;
	D3DXHANDLE				m_pLightDiffuseHandle;
	D3DXHANDLE				m_pLightSpecularHandle;
	D3DXHANDLE				m_pLightAttenuationHandle;
};