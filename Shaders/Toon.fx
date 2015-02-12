float4x4 world;
float4x4 worldViewProj;
float3	lightPos;
float3  lightDif;
float4	lineColor = { 0.f, 0.f, 0.f, 1.f};
float4  shadeColor = {0.f, 1.f, 0.f, 1.f};
float   lineThickness = .1;


struct A2V
{
	float3	posL	: POSITION0;
	float3  normalL : NORMAL0;
	float2  texCo	: TEXCOORD0;
};

struct VSOutput
{
	float4 posH	: POSITION0;
	float2 texCo	: TEXCOORD0;
	float3 normal	: TEXCOORD1;
	float3 posW	: TEXCOORD2;
};

VSOutput ToonVS(A2V IN)
{
	VSOutput OUT = (VSOutput)0;

	OUT.posH = mul(float4(IN.posL, 1.f), worldViewProj);
	OUT.posW = mul(float4(IN.posL, 1.f), world);
	OUT.normal = normalize(mul(IN.normalL, world));

	OUT.texCo = IN.texCo;

	return OUT;
}

float4	ToonPS(VSOutput IN) : COLOR
{
	IN.normal = normalize(IN.normal);
	float3 lightDir = lightPos - IN.posW;
	lightDir = normalize(lightDir);

	float intensity = saturate(dot(lightDir, IN.normal));

	float4 color = shadeColor * float4(lightDif, 1.f);
	color.a = 1.f;

	if (intensity > .95f)
		color = float4(1.f, 1.f, 1.f, 1.f) * color;
	else if (intensity > .5f)
		color = float4(.7f, .7f, .7f, 1.f) * color;
	else if (intensity > .05f)
		color = float4(.35f, .35f, .35f, 1.f) * color;
	else
		color = float4(.1f, .1f, .1f, 1.f) * color;

	return color;
}


VSOutput OutlineVS(A2V IN)
{
	VSOutput OUT = (VSOutput)0;

	float4 original = mul(float4(IN.posL, 1.f), worldViewProj);
	float4 normal = mul(float4(IN.normalL, 0.f), worldViewProj);

	OUT.posH = original + mul(normal, lineThickness);

	return OUT;
}

float4 OutlinePS(VSOutput IN) : COLOR
{
	return lineColor;
}

technique Toon
{
	pass p0
	{
		vertexShader = compile vs_3_0 OutlineVS();
		pixelShader = compile ps_3_0 OutlinePS();
		CullMode = CW;
	}

	pass p1
	{
		vertexShader = compile vs_3_0 ToonVS();
		pixelShader = compile ps_3_0 ToonPS();
		CullMode = CCW;
	}
}