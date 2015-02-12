#pragma once
#include "Effect.h"

//Just utilizes ambient light to display the mesh
class AmbientEffect : public Effect
{
public:
	//Constructor/Destructor
	AmbientEffect(NiteShader* parent, char* texture);
	virtual ~AmbientEffect();

	void Init() override;
	void Render(float dt, ID3DXMesh* mesh, int numMat) override;
	void Update(float dt);
	void Shutdown() override;
protected:
	//Texture info
	IDirect3DTexture9*			m_pTexture;
	char*						m_pTextureFile;
	//Handles
	D3DXHANDLE					m_pWorldViewProjHandle;
	D3DXHANDLE					m_pWorldHandle;
	D3DXHANDLE					m_pTextureHandle;
};