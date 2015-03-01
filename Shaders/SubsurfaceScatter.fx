float4x4	worldViewProj;
float4x4	lightViewProj;
float4x4	world;
float4x4	lightTexMatrix;
float4x4	lightMatrix;
float		grow;
texture		lightDepthTex;
texture		gradientTex;

sampler2D textureSampler = sampler_state
{
	Texture = <lightDepthTex>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

sampler scatterTex = sampler_state
{
	Texture = <gradientTex>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
	AddressU = CLAMP;
	AddressV = CLAMP;
};


struct A2V
{
	float4 Pos : POSITION0;
	float3 Normal : NORMAL0;
};

struct Depth2P
{
	float4 Pos : POSITION0;
	float4 Dist : TEXCOORD0;
	float3 WPos : TEXCOORD1;
};



float trace(float3 P, uniform float4x4 lightTexMatrix,
		float4x4 lightMatrix,
		sampler2D samp)
{
	//Transform point into texture space
	float4 texCoord = mul(float4(P, 1.0), lightTexMatrix);
	//Get distance from light at entry point
	float d_i = tex2Dproj(samp, texCoord); 
	//Transform position to light space
	float4 Plight = mul(float4(P, 1.0), lightMatrix);
	//distance of this pixel from the light
	float d_o = length(Plight);
	//calculate depth
	float s = (d_o - d_i)/5.f;
	return s;
}




Depth2P	DepthVS(A2V IN)
{
	//Zero out output structure
	Depth2P OUT = (Depth2P)0;
	//Store world position
	float4 P = IN.Pos;
	//Scale out the position
	P.xyz += IN.Normal * grow;
	OUT.Pos = mul(P, lightViewProj);
	OUT.Dist = length(mul(IN.Pos, lightMatrix));
	return OUT;
}

float4	DepthPS(Depth2P IN) : COLOR
{
	return IN.Dist;
}


Depth2P DrawVS(A2V IN)
{
	Depth2P OUT  = (Depth2P)0;
	OUT.WPos = IN.Pos;
	OUT.Pos = mul(IN.Pos, worldViewProj);
	return OUT;
}

float4	DrawPS(Depth2P IN) : COLOR
{
	float si = trace(IN.WPos, lightTexMatrix, lightMatrix, textureSampler);
	float4 color = tex1D(scatterTex, si);
	//return tex1D(scatterTex, si);
	return exp(-si * 1.5f) * color;
}


Technique SubSurfaceScatter
{	
	Pass p0
	{
		VertexShader = compile vs_3_0 DepthVS();
		PixelShader  = compile ps_3_0 DepthPS();
	}
	Pass p1
	{
		VertexShader = compile vs_3_0 DrawVS();
		PixelShader  = compile ps_3_0 DrawPS();
	}
}