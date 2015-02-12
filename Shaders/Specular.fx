

float4x4 world;										// World transform
float4x4 worldViewProj;								// World * View * Projection matrix
float3   eyePos;
texture  modelTexture;								// Texture
float3   lightPos;
float3   lightColor;
float4   MatColor = {1.0f, 1.0f, 1.0f, 1.0f};		// Color of the material
float    Ambient = 0.2f;

sampler textureSampler = sampler_state
{
    Texture = <modelTexture>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

//////////////////////////////////////////////////////////////
//Structures
//////////////////////////////////////////////////////////////

//Application to vertex shader
struct A2V
{ 
    float4 Pos : POSITION;
    float3 Normal : NORMAL;
    float2 Tex0 : TEXCOORD0;
};

//Vertex shader to pixel shader
struct V2P
{
    float4 Pos : POSITION;
    float4 WorldPos : TEXCOORD0;
    float3 WorldNormal : TEXCOORD1;
    float2 TexCoord0 : TEXCOORD2;
    float3 ViewDir : TEXCOORD3;
};

//////////////////////////////////////////////////////////////
//Vertex Shader
//////////////////////////////////////////////////////////////
void VS(in A2V IN, out V2P OUT)
{
    //Transform the position from view space to homogeneous projection space
    OUT.Pos = mul(IN.Pos, worldViewProj);

    //Compute world space position
    OUT.WorldPos = mul(IN.Pos, world);
    
    //Compute camera to light direction
    OUT.ViewDir = eyePos - OUT.WorldPos.xyz;

    // Compute world space normal
    OUT.WorldNormal = normalize(mul(IN.Normal, (float3x3)world));

    //Copy the texture coordinate as is
    OUT.TexCoord0 = IN.Tex0;
}

//////////////////////////////////////////////////////////////
//Pixel Shader
//////////////////////////////////////////////////////////////
float4 PS( in V2P IN) : COLOR0
{
    //Normalize the light vector
    float3 LightDir = lightPos - IN.WorldPos.rgb;
    LightDir = normalize(LightDir);
    IN.ViewDir = normalize(IN.ViewDir);
    
    //Compute lighting amount
    float4 I = min(saturate(dot(normalize(IN.WorldNormal), LightDir)) + Ambient, 1.0f);              
    
    //Compute specular values
    float3 Reflect = normalize(2 * I * IN.WorldNormal - LightDir);
    float4 Specular = pow(saturate(dot(Reflect, IN.ViewDir)), 16);
               
    //Lookup texture color
    float4 TexColor = tex2D(textureSampler, IN.TexCoord0);

    //Modulate texture color with lighting amount
    return float4(lightColor, 1.0f) * MatColor * TexColor * I + Specular;
}

//////////////////////////////////////////////////////////////
//Technique
//////////////////////////////////////////////////////////////
technique Specular
{
    pass P0
    {
	    ShadeMode = Flat;
        VertexShader = compile vs_2_0 VS();
        PixelShader  = compile ps_2_0 PS();
    }
}