#include "IShaderCompiler.h"
#include <unordered_map>
using namespace std;


class HLSLCompiler : public IShaderCompiler
{
public:
	HLSLCompiler(CreationFunction defaultFunction);
	~HLSLCompiler();
	virtual void SwapDefault(CreationFunction) ;
	virtual void PrintErrors();
	virtual Effect* CompileAndPrintErrors(const char*);
	virtual void Register(const char*, CreationFunction);
	virtual Effect* Compile(const char*);
private:
	HLSLCompiler() {}
	unordered_map<const char*, CreationFunction> m_creatorMap;
	ID3DXBuffer*		m_errors;
};