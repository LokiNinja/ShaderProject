//Default shaders
float4x4 worldViewProj;

struct Output
{
	float4 position : POSITION;
};

struct a2V
{
	float4 position : POSITION0;
};

Output	DefaultVS(a2V IN)
{
	Output output = (Output)0;
	output.position = mul(IN.position, worldViewProj);
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
		VertexShader = compile vs_3_0 DefaultVS();
		PixelShader = compile ps_3_0 DefaultPS();
		FillMode=WIREFRAME;
	}
}	