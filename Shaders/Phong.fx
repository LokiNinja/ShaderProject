
//Matrices
float4x4 	WorldViewProj;	
float4x4 	World;

float3		EyePos;			

//Light stuff
float3		LightPos;
float3		LightDif;		
float3		Specular;
float3		Attenuation;
//Ambient light
float3		Ambient = {1.f, 1.f, 1.f};		
//Material
float3 		AmbientMat = {1.f, 1.f, 1.f};
float3		DiffuseMat = {1.f, 1.f, 1.f};
float3		SpecularMat = {1.f, 1.f, 1.f};
float		SpecularPower = 30.f;

//Texture
texture Tex;
sampler TexSamp = sampler_state
{
	Texture = <Tex>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
};

//Output
struct OutputVS
{
	float4 PosH		: POSITION;
	float2 TexCo		: TEXCOORD0;
	float3 NormalW		: TEXCOORD1;
	float3 PosW		: TEXCOORD2;	
};



OutputVS mainVS(float3 pos : POSITION0,
		float3 normal : NORMAL0,
		float2 texCor : TEXCOORD0)
{
	//Zero out structure
	OutputVS output = (OutputVS)0;
	//Output homogenous coords
	output.PosH = mul(float4(pos, 1.f), WorldViewProj);
	//Transform Normals
	output.NormalW = mul(float4(normal, 0.f), World).xyz;
	output.NormalW = normalize(output.NormalW);
	//Get World Pos for pixel shader
	output.PosW = mul(float4(pos, 1.f), World).xyz;
	//Pass on texture coords
	output.TexCo = texCor;
	return output;
}

float4 mainPS(OutputVS input) : COLOR {
	//Normalize
	input.NormalW = normalize(input.NormalW);
	//Normalize 
	float3 toEye = normalize(EyePos - input.PosW);
	//Get the light and reflection(for specular) vectors
	float3 lightVec = normalize(LightPos - input.PosW);
	float3 r = reflect(-lightVec, input.NormalW);
	//Figure out the specular intensity
	float t = pow(saturate(dot(r, toEye)), SpecularPower);
	//Figure out diffuse intensity
	float intensity = saturate(dot(input.NormalW, lightVec));
	//Get texture color
	float4 texColor = tex2D(TexSamp, input.TexCo);
	//Figure out individual light components
	float3 diffuse = intensity * (DiffuseMat * LightDif * texColor).rgb;
	float3 ambient = (AmbientMat * Ambient * texColor).rgb;
	float3 spec = t * (SpecularMat * Specular).rgb;
	//Attenuate
	float d = distance(LightPos, input.PosW);
	float A = (Attenuation.x + d*Attenuation.y + d*d*Attenuation.z);
	//Return the final color
	float4 finalColor = float4((diffuse + ambient)/A, 1.f); 
	return finalColor;
}

technique Phong {
	pass p0 {
		CullMode = None;
		VertexShader = compile vs_3_0 mainVS();
		PixelShader = compile ps_3_0 mainPS();
	}
}
