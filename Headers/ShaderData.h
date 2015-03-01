#pragma once
#include <d3dx9.h>

class Light;
class Camera;

struct ShaderData
{
	const D3DXMATRIX*	world;
	const D3DXMATRIX*	view;
	const D3DXMATRIX*	proj;
	const Light*		light;
	const Camera*		camera;
};