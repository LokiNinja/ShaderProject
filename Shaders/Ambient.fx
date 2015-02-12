float4x4 worldViewProj : WORLDVIEWPROJ;
float4x4 world;
texture modelTexture;
float ambient = 1.0f;

// Texture sampler
sampler2D modelTextureSampler = sampler_state
{
    Texture = (modelTexture);
    MIPFILTER = LINEAR;
    MAGFILTER = LINEAR;
    MINFILTER = LINEAR;
};

// Application to vertex shader structure
struct a2v
{ 
    float4 position : POSITION0;
    float2 texCoord : TEXCOORD0;
};

// Vertex shader to pixel shader structure
struct v2p
{
    float4 position : POSITION0;
    float2 texCoord : TEXCOORD0;
};

//////////////////////////////////////////////////////////////
//Vertex Shader
//////////////////////////////////////////////////////////////
void vs(in a2v IN, out v2p OUT) 
{
    OUT.position = mul(IN.position, worldViewProj);
    OUT.texCoord = IN.texCoord;
}

//////////////////////////////////////////////////////////////
//Pixel Shader
//////////////////////////////////////////////////////////////
float4 ps(in v2p IN) : COLOR
{
    float4 TexColor = tex2D(modelTextureSampler, IN.texCoord);
    
    return TexColor * ambient;
}

//////////////////////////////////////////////////////////////
//Technique
//////////////////////////////////////////////////////////////
technique Ambient
{
    pass p0
    {
        vertexshader = compile vs_3_0 vs();
        pixelshader = compile ps_3_0 ps();
    }
}
