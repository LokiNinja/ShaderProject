#include "HLSLCompiler.h"
#include "Effect.h"
#include "Globals.h"


HLSLCompiler::HLSLCompiler(CreationFunction defaultFunction)
{
	m_creatorMap["default"] = defaultFunction;
	m_errors = nullptr;
}

HLSLCompiler::~HLSLCompiler()
{
	SAFE_RELEASE(m_errors);
}


void HLSLCompiler::SwapDefault(CreationFunction newDefault)
{
	m_creatorMap["default"] = newDefault;
}

void HLSLCompiler::PrintErrors()
{
	if (m_errors)
	{
		string newString((char*)m_errors->GetBufferPointer());
		wstring tempString(newString.begin(), newString.end());
		MessageBox(0, tempString.c_str(), L"ERROR", MB_OK);
	}
}

Effect* HLSLCompiler::CompileAndPrintErrors(const char* effectkey)
{
	Effect* thisEffect = Compile(effectkey);
	if (m_errors)
	{
		PrintErrors();
		thisEffect = Compile("default");
	}
	return thisEffect;
}

void HLSLCompiler::Register(const char* effectKey, CreationFunction creator)
{
	auto it = m_creatorMap.find(effectKey);
	if (it == m_creatorMap.end())
	{
		m_creatorMap[effectKey] = creator;
	}
}

Effect* HLSLCompiler::Compile(const char* effectkey)
{
	auto creatorIt = m_creatorMap.find(effectkey);
	CreationFunction creator = nullptr;
	if (creatorIt == m_creatorMap.end())
	{
		return nullptr;
	}
	else
	{
		creator = creatorIt->second;
	}
	Effect* thisEffect = creator(&m_errors);
	if (m_errors)
	{
		delete thisEffect;
		return nullptr;
	}
	thisEffect->Init();
	return thisEffect;
}
