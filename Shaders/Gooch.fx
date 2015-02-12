//Gooch shader
float4x4	worldViewProj;
float4x4	world;
float3		warmColor;
float3		coolColor;
float3		lightPos;


struct OutVS
{
	float4 posH : POSITION0;
	float3 normalW : TEXCOORD0;
	float3 lightVec : TEXCOORD1;
};


OutVS	GoochVS(float3 position : POSITION0,float3 normal : NORMAL0)
{
	//Initialize output
	OutVS OUT = (OutVS)0;
	//Homogeneous coordinate
	OUT.posH = mul(float4(position, 1.f), worldViewProj);
	//Transform the normals, just use worldViewProj because scaling is all uniform
	OUT.normalW = mul(float4(normal, 0.f), worldViewProj).xyz;
	//Find light vector, first step is determining world position of vertex
	float4 pos = mul(float4(position, 1.f), world);
	OUT.lightVec = (lightPos - pos.rgb);	
	return OUT;
}


float4	GoochPS(OutVS IN) : COLOR
{
	//normalize vectors
	IN.normalW = normalize(IN.normalW);
	IN.lightVec = normalize(IN.lightVec);
	float intensity = dot(IN.normalW, IN.lightVec);
	intensity = .5f * (intensity + 1.0f);
	float4 color1 = float4(coolColor, 1.f);
	float4 color2 = float4(warmColor, 1.f);
	float4 result = lerp(color1, color2, intensity);
	return result;
}



technique Gooch
{
	pass p0
	{
		VertexShader = compile vs_2_0 GoochVS();
		PixelShader = compile ps_2_0 GoochPS();
	}
}