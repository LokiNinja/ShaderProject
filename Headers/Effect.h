#pragma once
#include <d3dx9.h>

class NiteShader;

class Effect
{
public:
	Effect(NiteShader* parent);
	virtual ~Effect();

	virtual void Init() = 0;
	virtual void Render(float dt, ID3DXMesh* mesh, int numMat) = 0;
	virtual void Shutdown();
	virtual void Update(float dt) = 0;
	virtual void ReCompile();

	virtual void OnLostDevice();
	virtual void OnResetDevice();
	
protected:
	NiteShader*			m_pParent;
	ID3DXEffect*		m_pEffect;
	char*				m_pFileName;
	char*				m_pEffectName;
	ID3DXBuffer*		m_pErrors;
	D3DXHANDLE			m_pTechniqueHandle;
	bool				m_bCompileErrors;

	void PrintName();
	void CompileShader();
	void CompileDefault();
};