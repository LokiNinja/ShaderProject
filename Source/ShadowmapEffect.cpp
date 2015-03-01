#include "ShadowmapEffect.h"
#include "Globals.h"
#include "Camera.h"
#include "ShaderData.h"
#include "Light.h"
#include <climits>
using namespace std;

#define GROUNDSIZE 40.f
#define SHADOW_MAP_SIZE 512

//Constructor
ShadowmapEffect::ShadowmapEffect( const WCHAR* sphereTex, const WCHAR* groundTex)
{
	m_pSphereTexture = 0;
	m_pSphereTextureFile = sphereTex;
	m_pGroundTexture = 0;
	m_pGroundTextureFile = groundTex;
	m_pShadowMapSurface = 0;
	m_pGroundVB = 0;
	m_pGroundIB = 0;
	m_pShadowMapTexture = 0;

	//Handles
	m_pWorldViewProjHandle = 0;
	m_pWorldHandle = 0;
	m_pTextureHandle = 0;
	m_pTextureBiasHandle = 0;
	m_pShadowMapHandle = 0;
	m_pLightSpaceHandle = 0;
	m_pLightViewProjHandle = 0;
	m_pShadowMapResolutionHandle = 0;
}

//Destructor
ShadowmapEffect::~ShadowmapEffect()
{
	Shutdown();
}

//Initialize
void ShadowmapEffect::Init()
{
	//Create Identity matrices
	D3DXMatrixIdentity(&m_lightSpace);
	D3DXMatrixIdentity(&m_textureSpace);
	D3DXMatrixIdentity(&m_textureBias);
	D3DXMatrixIdentity(&m_lightProj);

	//Set up ground geometry
	InitGround();

	//Get Handles
	m_pWorldViewProjHandle = m_pEffect->GetParameterByName(0, "worldViewProj");
	m_pWorldHandle = m_pEffect->GetParameterByName(0, "world");
	m_pLightSpaceHandle = m_pEffect->GetParameterByName(0, "lightSpace");
	m_pLightViewProjHandle = m_pEffect->GetParameterByName(0, "lightViewProj");
	m_pTextureBiasHandle = m_pEffect->GetParameterByName(0, "textureBias");
	m_pTextureHandle = m_pEffect->GetParameterByName(0, "tex");
	m_pShadowMapHandle = m_pEffect->GetParameterByName(0, "shadowMap");
	m_pShadowMapResolutionHandle = m_pEffect->GetParameterByName(0, "shadowMapRes");

	HR(D3DXCreateTextureFromFile(m_pDevice, m_pSphereTextureFile, &m_pSphereTexture));
	HR(D3DXCreateTextureFromFile(m_pDevice, m_pGroundTextureFile, &m_pGroundTexture));
	
	//Set up technique
	m_pTechniqueHandle = m_pEffect->GetTechniqueByName("ShadowMap");
	HR(m_pEffect->SetTechnique(m_pTechniqueHandle));

	//Initialize render to texture
	HR(D3DXCreateTexture(m_pDevice, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, 1,
						D3DUSAGE_RENDERTARGET, D3DFMT_A32B32G32R32F, D3DPOOL_DEFAULT, &m_pShadowMapTexture));
	HR(m_pShadowMapTexture->GetSurfaceLevel(0, &m_pShadowMapSurface));

	//Set constant data members in the shader
	HR(m_pEffect->SetMatrix(m_pLightSpaceHandle, &m_lightSpace));
	HR(m_pEffect->SetMatrix(m_pLightViewProjHandle, &(m_lightSpace * m_lightProj)));
	HR(m_pEffect->SetMatrix(m_pTextureBiasHandle, &m_textureBias));
	HR(m_pEffect->SetFloat(m_pShadowMapResolutionHandle, (float)SHADOW_MAP_SIZE));
}

//Cleanup
void ShadowmapEffect::Shutdown()
{
	SAFE_RELEASE(m_pSphereTexture);
	SAFE_RELEASE(m_pGroundTexture);
	SAFE_RELEASE(m_pShadowMapTexture);
	SAFE_RELEASE(m_pGroundIB);
	SAFE_RELEASE(m_pGroundVB);
	SAFE_RELEASE(m_pShadowMapSurface);
}

//Render
void ShadowmapEffect::Render(float dt, ID3DXMesh* mesh, int numMaterials)
{
	HR(m_pDevice->BeginScene());
	IDirect3DSurface9*			m_pBackBuffer = nullptr;
	UINT passes;
	//Set data for this pass
	HR(m_pEffect->SetMatrix(m_pLightViewProjHandle, &(m_lightSpace * m_lightProj)));
	HR(m_pEffect->SetMatrix(m_pLightSpaceHandle, &m_lightSpace));

	//Get the backbuffer pointer
	HR(m_pDevice->GetRenderTarget(0, &m_pBackBuffer));
	//Set the render target for depth map
	HR(m_pDevice->SetRenderTarget(0, m_pShadowMapSurface));
	//Clear the render target
	HR(m_pDevice->Clear(0, 0, D3DCLEAR_TARGET, D3DXCOLOR(1.f, 1.f, 1.f, 1.f), 1.f, 0));

	//Depth Pass -- Begin
	HR(m_pEffect->Begin(&passes, 0));
	HR(m_pEffect->BeginPass(0));
	//Draw Ground
	DrawGround();
	//Draw Sphere
	for (int i = 0; i < numMaterials; i++)
		HR(mesh->DrawSubset(i));
	HR(m_pEffect->EndPass());
	//Depth Pass -- End

	//Switch Render targets to back buffer
	HR(m_pDevice->SetRenderTarget(0, m_pBackBuffer));
	//Just clear zbuffer, should already be cleared from NiteShader::Render
	HR(m_pDevice->Clear(0, 0, D3DCLEAR_ZBUFFER, D3DXCOLOR(0.f, 0.f, 0.f, 1.f), 1.f, 0));
	//Set the texture for the ground
	HR(m_pEffect->SetTexture(m_pTextureHandle, m_pGroundTexture));
	HR(m_pEffect->SetTexture(m_pShadowMapHandle, m_pShadowMapTexture));

	//Draw Pass -- Begin
	HR(m_pEffect->BeginPass(1));
	//Draw ground
	DrawGround();
	//Set sphere texture
	HR(m_pEffect->SetTexture(m_pTextureHandle, m_pSphereTexture));
	HR(m_pEffect->CommitChanges());
	//Draw Sphere
	for (int i = 0; i < numMaterials; i++)
		HR(mesh->DrawSubset(i));
	HR(m_pEffect->EndPass());
	HR(m_pEffect->End());
	//Draw Pass -- End

	//Print Effect Name
	SAFE_RELEASE(m_pBackBuffer);
	HR(m_pDevice->EndScene());
}

void ShadowmapEffect::SetData(const ShaderData* data)
{
	UpdateLightView(data->light);
	UpdateLightProj();
	InitTextureBias();
	D3DXMATRIX WVP = (*data->world) * *(data->view) * *(data->proj);
	HR(m_pEffect->SetMatrix(m_pWorldViewProjHandle, &WVP));
	HR(m_pEffect->SetMatrix(m_pWorldHandle, data->world));
}



//HELPERS 
//Load data into ground plane VB and IB
void ShadowmapEffect::InitGround()
{
	HR(m_pDevice->CreateVertexBuffer(4 * sizeof(Vertex), 0, 0, D3DPOOL_MANAGED, &m_pGroundVB, 0));
	HR(m_pDevice->CreateIndexBuffer(6 * sizeof(WORD), 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pGroundIB, 0));

	WORD* i = 0;
	Vertex* v = 0;
	HR(m_pGroundVB->Lock(0, 0, (void**)&v, 0));

	//Lowerleft
	v[0].pos = D3DXVECTOR3(-GROUNDSIZE, -5.f, -GROUNDSIZE);
	v[0].normal = D3DXVECTOR3(0.f, 1.f, 0.f);
	v[0].texture = D3DXVECTOR2(0.f, 2.f);

	//Upper left
	v[1].pos = D3DXVECTOR3(-GROUNDSIZE, -5.f, GROUNDSIZE);
	v[1].normal = D3DXVECTOR3(0.f, 1.f, 0.f);
	v[1].texture = D3DXVECTOR2(0.f, 0.f);

	//upper right
	v[2].pos = D3DXVECTOR3(GROUNDSIZE, -5.f, GROUNDSIZE);
	v[2].normal = D3DXVECTOR3(0.f, 1.f, 0.f);
	v[2].texture = D3DXVECTOR2(2.f, 0.f);

	//lower right
	v[3].pos = D3DXVECTOR3(GROUNDSIZE, -5.f, -GROUNDSIZE);
	v[3].normal = D3DXVECTOR3(0.f, 1.f, 0.f);
	v[3].texture = D3DXVECTOR2(2.f, 2.f);

	HR(m_pGroundVB->Unlock());


	HR(m_pGroundIB->Lock(0, 0, (void**)&i, 0));
	//Triangle 1
	i[0] = 0;
	i[1] = 1;
	i[2] = 2;

	//Triangle 2
	i[3] = 0;
	i[4] = 2;
	i[5] = 3;
	HR(m_pGroundIB->Unlock());
}

//Draws the ground
void ShadowmapEffect::DrawGround()
{
		HR(m_pDevice->SetStreamSource(0, m_pGroundVB, 0, sizeof(Vertex)));
		HR(m_pDevice->SetIndices(m_pGroundIB));
		HR(m_pDevice->SetVertexDeclaration(Vertex::decl));
		HR(m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 6, 0, 2));
}

//Initialize the combination projection and texture bias matrix
void ShadowmapEffect::InitTextureBias()
{
	D3DXMATRIX temp;
	D3DXMatrixScaling(&temp, .5f, .5f, .5f);
	m_textureBias *= temp;
	D3DXMatrixTranslation(&temp, .5f, .5f, .5f);
	m_textureBias *= temp;
}

void ShadowmapEffect::UpdateLightView(const Light* light)
{
	//Set up light view -- initial vectors (not normalized or even correcly oriented yet)
	D3DXVECTOR3 right;
	D3DXVECTOR3 up(0.f, 1.f, 0.f);
	D3DXVECTOR3 pos = light->GetPosition();
	//Looking at origin so look at is simply negative pos
	D3DXVECTOR3 lookat = -pos;
	D3DXVec3Normalize(&lookat, &lookat);
	//Cross product to get the right vector
	D3DXVec3Cross(&right, &up, &lookat);
	D3DXVec3Normalize(&right, &right);
	//Find the up vector
	D3DXVec3Cross(&up, &lookat, &right);
	D3DXVec3Normalize(&up, &up);
	//Set up the matrix
	D3DXMatrixLookAtLH(&m_lightSpace, &pos, &lookat, &up);
}

void ShadowmapEffect::UpdateLightProj()
{
	//To hold min and max values of the frustum
	float minX, minY, minZ;
	float maxX, maxY, maxZ;

	//To hold vertex data
	Vertex* v = 0;
	D3DVERTEXBUFFER_DESC desc;

	//Lock the buffer
	HR(m_pGroundVB->Lock(0, 0,(void**) &v, 0));

	//Get the number of vertices
	HR(m_pGroundVB->GetDesc(&desc));
	float size = desc.Size/sizeof(Vertex);

	//Initialize starting values
	D3DXVECTOR3 initPos = v[0].pos;
	minX = maxX = initPos.x;
	minY = maxY = initPos.y;
	minZ = maxZ = initPos.z;

	//Loop through and find min and max values
	for (UINT i = 0; i < size; i++)
	{
		D3DXVECTOR4 temp;
		D3DXVec3Transform(&temp, &v[i].pos, &m_lightSpace);

		if (temp.x < minX)
			minX = temp.x;
		if (temp.x > maxX)
			maxX = temp.x;
		if (temp.y < minY)
			minY = temp.y;
		if (temp.y > maxY)
			maxY = temp.y;
		if (temp.z < minZ)
			minZ = temp.z;
		if (temp.z > maxZ)
			maxZ = temp.z;
	}
	//Unlock
	HR(m_pGroundVB->Unlock());
	
	D3DXMatrixOrthoLH(&m_lightProj, maxX - minX + 10, maxY - minY + 10,  minZ - 10, maxZ + 10); 
}

Effect* ShadowmapEffect::Create(ID3DXBuffer** errors)
{
	ShadowmapEffect* newEffect = NEW ShadowmapEffect(L"Textures/Grass.bmp", L"Textures/floor.bmp");
	D3DXCreateEffectFromFile(m_pDevice, L"Shaders/Shadowmap.fx", 0, 0, D3DXSHADER_DEBUG, 0, &newEffect->m_pEffect, errors);
	return newEffect;
}
