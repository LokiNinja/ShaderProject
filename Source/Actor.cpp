#include "Actor.h"
#include "Globals.h"
#include "NiteShader.h"

//Constructor
Actor::Actor(NiteShader* parent)
{
	m_pos = D3DXVECTOR3(0.f, 0.f, 0.f);
	m_scale = D3DXVECTOR3(1.f, 1.f, 1.f);
	m_rotation = D3DXVECTOR3(0.f, 0.f, 0.f);
	m_pMesh = 0;
	m_pAdjacency = 0;
	m_pParent = parent;
	m_numMaterials = 0;
}

//Destructor
Actor::~Actor()
{
	SAFE_RELEASE(m_pMesh);
	delete [] m_pAdjacency;
	m_pAdjacency = 0;
}

//Loads a mesh
void Actor::Load(char* fileName)
{
	//Reset everything
	SAFE_RELEASE(m_pMesh);
	delete [] m_pAdjacency;
	m_pAdjacency = 0;
	m_pos = D3DXVECTOR3(0.f, 0.f, 0.f);
	m_scale = D3DXVECTOR3(1.f, 1.f, 1.f);
	m_rotation = D3DXVECTOR3(0.f, 0.f, 0.f);

	if (fileName == NULL)
	{
		CreateSphere();
	}
	else
	{
		CreateFromX(fileName);
	}
}


//Creates a sphere mesh
void Actor::CreateSphere()
{
	HR(D3DXCreateSphere(m_pParent->GetDevice(), 5.f, 40, 40, &m_pMesh, 0));
	m_pAdjacency = new DWORD[m_pMesh->GetNumFaces() * 3];
	HR(m_pMesh->GenerateAdjacency(0.001f, m_pAdjacency));
	GenerateSphereVertex();
	m_numMaterials = 1;
}

//Loads mesh from x file
void Actor::CreateFromX(char* fileName)
{
}

//Generate Sphere Vertex
void Actor::GenerateSphereVertex()
{
	//Generate Texture coordinates
	D3DVERTEXELEMENT9	definition[64];
	UINT				numElements;

	Vertex::decl->GetDeclaration(definition, &numElements);

	//Clone Mesh with new vertex declaration
	ID3DXMesh*			temp = 0;
	HR(m_pMesh->CloneMesh(D3DXMESH_SYSTEMMEM, definition, m_pParent->GetDevice(), &temp));

	SAFE_RELEASE(m_pMesh);

	//Lock buffer
	Vertex*	vertices = 0;
	HR(temp->LockVertexBuffer(0, (void**)&vertices));

	//Iterate through, generate the spherical mapping
	for (UINT i = 0; i < temp->GetNumVertices(); i++)
	{
		D3DXVECTOR3 pos = vertices[i].pos;

		//Counterclockwise angle
		float theta = atan2f(pos.z, pos.x);
		//Vertical angle
		float phi = acosf(pos.y / sqrt(pos.x * pos.x + pos.y * pos.y + pos.z * pos.z));

		float u = theta /(2.0f*D3DX_PI);
		float v = phi / D3DX_PI;

		vertices[i].texture.x = u;
		vertices[i].texture.y = v;
	}

	HR(temp->UnlockVertexBuffer());
	//Compute tangent basis and normals
	HR(D3DXComputeTangentFrameEx(temp, D3DDECLUSAGE_TEXCOORD, 0,
								D3DDECLUSAGE_BINORMAL, 0,
								D3DDECLUSAGE_TANGENT, 0,
								D3DDECLUSAGE_NORMAL, 0,
								0,
								0,
								.01f, .25f, .01f, &m_pMesh,0));

	SAFE_RELEASE(temp);
}