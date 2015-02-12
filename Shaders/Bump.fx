float4x4	worldViewProj;
float4x4	world;
float4x4	worldInv;
float3		lightPos;
float3		lightDif;
float3		eyePosW;
float3		attenuation;
texture		tex;
texture		normalMap;

float3		ambientLight = {1.f, 1.f, 1.f};
float3		ambientMat = {.8f, .8f, .8f};
float3		diffuseMat = {.8f, .8f, .8f};

sampler TexS = sampler_state
{
	Texture = <tex>;
	MinFilter = ANISOTROPIC;
	MaxAnisotropy = 8;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

sampler NormalMapS = sampler_state
{
	Texture = <normalMap>;
	MinFilter = ANISOTROPIC;
	MaxAnisotropy = 8;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};


struct A2V
{
	float3 posL		: POSITION0;
	float3 tangentL		: TANGENT0;
	float3 binormalL	: BINORMAL0;
	float3 normalL		: NORMAL0;
	float2 texCo		: TEXCOORD0;
};

struct OutputVS
{
	float4 posH		: POSITION0;
	float3 toEyeT		: TEXCOORD0;
	float3 lightDirT	: TEXCOORD1;
	float3 posW		: TEXCOORD3;
	float2 texCo		: TEXCOORD2;
};

//Vertex Shader
OutputVS BumpVS(A2V IN)
{
	OutputVS OUT = (OutputVS)0;

	//TBN basis
	float3x3 TBN;
	TBN[0] = IN.tangentL;
	TBN[1] = IN.binormalL;
	TBN[2] = IN.normalL;

	//Transform from object to tangent space
	float3x3 toTangentSpace = transpose(TBN);

	//Transform eye pos to local space
	float3 eyePosL = mul(float4(eyePosW, 1.f), worldInv);

	//Transform eye vector to tangent
	float3 toEyeL = eyePosL - IN.posL;
	OUT.toEyeT = mul(float4(toEyeL, 0.f), toTangentSpace);

	//Transform light direction to tangent space
	float3 lightDirL = mul(float4(lightPos, 1.f), worldInv);
	lightDirL = lightDirL - IN.posL;
	OUT.lightDirT = mul(float4(lightDirL, 0.f), toTangentSpace);

	//Get world position
	OUT.posW = mul(float4(IN.posL, 1.f), world);

	//Transform to homogeneous clip space
	OUT.posH = mul(float4(IN.posL, 1.f), worldViewProj);

	//Pass texture coordinates through
	OUT.texCo = IN.texCo;

	return OUT;
}

//Pixel Shader
float4	BumpPS(OutputVS IN) : COLOR
{
	//renormalize interpolated vectors
	IN.toEyeT = normalize(IN.toEyeT);
	IN.lightDirT = normalize(IN.lightDirT);

	//light vector is opposite the direction of the light
	float3 lightVecT = IN.lightDirT;

	//sample normal map
	float3 normalT = tex2D(NormalMapS, IN.texCo);
	
	//Expand to correct interval
	normalT = 2.f*normalT - 1.f;

	normalT = normalize(normalT);

	//Calculate intensity
	float intensity = saturate(dot(normalT, lightVecT));
	//Get texture color
	float4 texColor = tex2D(TexS, IN.texCo);
	//Calculate diffuse
	float3 diffuse = intensity * (diffuseMat * lightDif * texColor).rgb;
	//calculate ambient
	float3 ambient = (ambientMat* ambientLight * texColor).rgb;
	//calculate attenuation
	float d = distance(lightPos, IN.posW);
	float a = (attenuation.x + d*attenuation.y + d*d*attenuation.z);

	//compute final color
	float4 finalColor = float4((diffuse+ambient)/a, 1.f);
	return finalColor;
}


technique	Bump
{
	pass p0
	{
		vertexShader = compile vs_3_0 BumpVS();
		pixelShader = compile ps_3_0 BumpPS();
	}
}