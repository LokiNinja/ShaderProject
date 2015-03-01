#pragma once
#include <d3dx9.h>

class Camera
{
public:
	//Constructor/Destructor
	Camera(D3DXVECTOR3);
	~Camera();

	//Accessors/mutators
	const D3DXMATRIX&	GetView() const;
	const D3DXMATRIX&	GetProjection() const;
	const D3DXMATRIX&	GetViewProj() const;

	//Returns the basis vectors
	const D3DXVECTOR3&	GetCameraZ() const;
	const D3DXVECTOR3&	GetCameraX() const;
	const D3DXVECTOR3&	GetCameraY() const;


	//Sets the camera speed
	void				SetSpeed(float newSpeed);

	//Returns the position of the camera
	const D3DXVECTOR3&		GetPos() const;

	//update function
	void				Update(float dt);
	//Sets the camera to look at a point
	void				LookAt(D3DXVECTOR3& pos, D3DXVECTOR3& target, D3DXVECTOR3& up);
	//Sets the projetion
	void SetProjection(float fov, float aspect, float nearZ, float farZ);
private:
	//Stores various matrices: view, projection, viewproj
	D3DXMATRIX	m_view;
	D3DXMATRIX	m_proj;
	D3DXMATRIX	m_viewProj;

	//Holds the basis vectors for camera space
	D3DXVECTOR3 m_cameraZ;
	D3DXVECTOR3 m_cameraX;
	D3DXVECTOR3 m_cameraY;
	//Camera position in world space
	D3DXVECTOR3	m_position;

	//Camera movement speed
	float m_speed;

	//Builds the view matrix
	void BuildView();
};