#pragma once
#include "Effect.h"

//Just utilizes ambient light to display the mesh
class AmbientEffect : public Effect
{
public:
	//Constructor/Destructor

	virtual ~AmbientEffect();
	static Effect* Create(ID3DXBuffer**);
	void Init() override;
	void Render(float dt, ID3DXMesh* mesh, int numMat) override;
	void SetData(const ShaderData*) override;
	void Shutdown() override; 
private:
	AmbientEffect(const WCHAR* texture);
	AmbientEffect() {}

	//Texture info
	IDirect3DTexture9*			m_pTexture;
	const WCHAR*				m_pTextureFile;
	//Handles
	D3DXHANDLE					m_pWorldViewProjHandle;
	D3DXHANDLE					m_pWorldHandle;
	D3DXHANDLE					m_pTextureHandle;
};