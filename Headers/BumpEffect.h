#pragma once
#include "Effect.h"


class BumpEffect : public Effect
{
public:
	//Constructor/Destructor

	~BumpEffect();
	static Effect* Create(ID3DXBuffer**);
	void Init() override;
	void Shutdown() override;
	void Render(float dt, ID3DXMesh* mesh, int numMaterials) override;
	void SetData(const ShaderData* data) override;
private:
	BumpEffect(const WCHAR* texture, const WCHAR* normalMap);
	BumpEffect() {}

	//Texture and normal map stuff
	IDirect3DTexture9*		m_pTexture;
	const WCHAR*			m_pTextureFile;
	IDirect3DTexture9*		m_pNormalMap;
	const WCHAR*			m_pNormalMapFile;

	D3DXHANDLE				m_pWorldViewProjHandle;
	D3DXHANDLE				m_pWorldHandle;
	D3DXHANDLE				m_pLightDiffuseHandle;
	D3DXHANDLE				m_pLightAttenuationHandle;
	D3DXHANDLE				m_pWorldInvHandle;
	D3DXHANDLE				m_pLightDirHandle;
	D3DXHANDLE				m_pEyePosHandle;
	D3DXHANDLE				m_pTextureHandle;
	D3DXHANDLE				m_pNormalMapHandle;
};