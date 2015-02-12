//Default shaders

struct Output
{
	float4 position : POSITION;
};

Output	DefaultVS(float3 pos : POSITION)
{
	Output output = (Output)0;
	output.position = float4(pos, 1.f);
	return output;
}

float4 DefaultPS(Output input) : COLOR
{
	return float4 (1.f, 1.f, 1.f, 1.f);
}

technique Default
{
	pass p0
	{
		VertexShader = compile vs_2_0 DefaultVS();
		PixelShader = compile ps_2_0 DefaultPS();
	}
}	