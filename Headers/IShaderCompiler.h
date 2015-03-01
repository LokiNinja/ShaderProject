#pragma once
#include <d3d9.h>
#include <d3dx9.h>

class Effect;

typedef Effect*(*CreationFunction)(ID3DXBuffer**);

class IShaderCompiler
{
public:
	virtual void SwapDefault(CreationFunction) = 0;
	virtual void PrintErrors() = 0;
	virtual Effect* CompileAndPrintErrors(const char*) = 0;
	virtual void Register(const char*, CreationFunction) = 0;
	virtual Effect* Compile(const char*) = 0;
};