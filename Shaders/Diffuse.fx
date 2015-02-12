float4x4 worldViewProj;
float4x4 world;
texture modelTexture;
float3 lightPos;
float ambient = 0.2f;

// Texture samler
sampler2D modelTextureSampler = sampler_state
{
    Texture = (modelTexture);
    MIPFILTER = LINEAR;
    MAGFILTER = LINEAR;
    MINFILTER = LINEAR;
};

//////////////////////////////////////////////////////////////
//Structures
//////////////////////////////////////////////////////////////

//Application to vertex shader
struct A2V
{ 
    float4 position : POSITION0;
    float3 normal   : NORMAL0;
    float2 texCoord : TEXCOORD0;
};

//Vertex shader to pixel shader
struct V2P
{
    float4 position : POSITION0;
    float2 texCoord : TEXCOORD0;
    float3 color    : TEXCOORD1;
};

//////////////////////////////////////////////////////////////
//Vertex Shader
//////////////////////////////////////////////////////////////
void VS( in A2V IN, out V2P OUT) 
{
    //Get light direction vector
    float3 posW = mul(IN.position, world).rgb;
    float3 lightPosW = mul(lightPos, world).rgb;
    float3 lightDir = lightPosW - posW;
    lightDir = normalize(lightDir);
    OUT.position = mul(IN.position, worldViewProj);
    IN.normal    = normalize(mul(IN.normal, world));
    OUT.color    = max(dot(IN.normal, normalize(lightDir)),0.0f) + ambient;
    OUT.texCoord = IN.texCoord;
}

//////////////////////////////////////////////////////////////
//Pixel Shader
//////////////////////////////////////////////////////////////
float4 PS( in V2P IN) : COLOR
{
    float4 TexColor = tex2D( modelTextureSampler, IN.texCoord );
    
    return TexColor * float4(IN.color, 1.0f);
}

//////////////////////////////////////////////////////////////
//Techniques
//////////////////////////////////////////////////////////////
technique Diffuse
{
    pass p0
    {
		ShadeMode = Flat;
        vertexshader = compile vs_2_0 VS();
        pixelshader = compile ps_2_0 PS();
    }
}