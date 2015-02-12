//These change
float4x4	worldViewProj;
float4x4	world;
texture		tex;

//These remain constant
float4x4	lightSpace;
float4x4	lightViewProj;
float4x4	textureBias;
texture		shadowMap;

float		shadowMapRes;
float		ambient = .8;
float		bias = .0001;

sampler TexS = sampler_state
{
	Texture = <tex>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

sampler TexShadow = sampler_state
{
	Texture = <shadowMap>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
};

struct A2V
{	
	float3 posL	: POSITION0;
	float2 texCo	: TEXCOORD0;
};

struct OutputVS
{
	float4 posH	: POSITION0;
	float2 texCo	: TEXCOORD0;
	float4 posW	: TEXCOORD2;
};


//Helper function for percentage closer filtering 3x3
// 6 7 8
// 1 0 2
// 3 4 5
float PCF(float2 texCoord)
{
      	float2 texCoords[9];
	float texelSize = 1.f/shadowMapRes;

	texCoords[0] = texCoord;
	texCoords[1] = texCoord + float2(-texelSize, 0.f);
	texCoords[2] = texCoord + float2(texelSize, 0.f);
	texCoords[3] = texCoord + float2(-texelSize, texelSize);
	texCoords[4] = texCoord + float2(0.f, texelSize);
	texCoords[5] = texCoord + float2(texelSize, texelSize);
	texCoords[6] = texCoord + float2(-texelSize, -texelSize);
	texCoords[7] = texCoord + float2(0.f, -texelSize);
	texCoords[8] = texCoord + float2(texelSize, -texelSize);

	float depth = 0.f;

	for (int i = 0; i < 9; i++)
	{
		depth += tex2D(TexShadow, texCoords[i]);
	}

	return depth / 9.f;
}

//Vertex Shader Depth Pass
OutputVS DepthVS(A2V IN)
{
	OutputVS OUT = (OutputVS)0;
	
	//Get screen coordinates in light space for texture map
	OUT.posH = mul(float4(IN.posL, 1.f), lightViewProj);
	//Get the depth by performing a perspective divide on the projected coordinates
	OUT.posW.x = OUT.posH.z;
	return OUT;
}


//Pixel shader depth Pass
float4	DepthPS(OutputVS IN) : COLOR
{
	//Texture only uses red channel, just store it there
	return float4(IN.posW.x, 0, 0, 1);
}


//VertexShader Draw Pass
OutputVS DrawVS(A2V IN)
{
	OutputVS OUT = (OutputVS)0;

	//Get the screen coordinates for this pixel
	OUT.posH = mul(float4(IN.posL, 1.f), worldViewProj);
	//Send texture coordinates through
	OUT.texCo = IN.texCo;
	//Pass its world coordinates through
	OUT.posW = mul(float4(IN.posL, 1.f), world);
	return OUT;
}

//PixelShader Draw Pass
float4 DrawPS(OutputVS IN) : COLOR
{	
	//Get the pixels screen position in light space
	float4 texCoord = mul(IN.posW, lightViewProj);
	//Multiply by texture bias to bring in range 0-1
	texCoord = mul(texCoord, textureBias);
	texCoord.y = -texCoord.y;
	//Get corresponding depth value using PCF
	float prevDepth = PCF(texCoord);
	//Check if it is in shadow
	float4 posLight = mul(IN.posW, lightViewProj);
	float currDepth = posLight.z - bias;
	if (currDepth > prevDepth)
	{
		return tex2D(TexS, IN.texCo) * (1.f-prevDepth) * ambient;
	}
	else
		return tex2D(TexS, IN.texCo) * ambient;	
}


//Effect info
technique ShadowMap
{
	pass p0
	{
		vertexShader = compile vs_2_0 DepthVS();
		pixelShader = compile ps_2_0 DepthPS();
	}

	pass p1
	{
		vertexShader = compile vs_2_0 DrawVS();
		pixelShader = compile ps_2_0 DrawPS();
	}
}