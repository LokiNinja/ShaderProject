#pragma once
#include <d3dx9.h>

class NiteShader;

class Actor
{
public:
	//Transform data
	D3DXVECTOR3		m_pos;
	D3DXVECTOR3		m_scale;
	D3DXVECTOR3		m_rotation;

	//Constructor
	Actor(NiteShader* parent);
	//Destructor
	~Actor();

	//Loads the mesh -- creates a sphere if argument is NULL
	void Load(char* fileName);
	//Gets a pointer to the mesh
	ID3DXMesh*	GetMesh() const { return m_pMesh; }
	int			GetNumMaterials() const { return m_numMaterials; }
private:
	//The physical mesh
	ID3DXMesh*		m_pMesh;
	//Adjacency info
	DWORD*			m_pAdjacency;
	//Pointer to parent app
	NiteShader*		m_pParent;
	//Number of subsets/materials
	int				m_numMaterials;

	//Creates a sphere
	void CreateSphere();
	//Loads from X file
	void CreateFromX(char* fileName);
	//Generate Vertex Info Sphere
	void GenerateSphereVertex();
};