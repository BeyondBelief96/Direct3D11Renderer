
cbuffer cbuf
{
    matrix modelViewProj;
};

struct VSOut
{
    float2 texCoord : TEXCOORD;
    float4 position : SV_Position;
};

VSOut main(float3 pos : POSITION, float2 texCoord : TEXCOORD)
{
    VSOut vso;
    vso.position = mul(float4(pos, 1.0f), modelViewProj);
    vso.texCoord = texCoord;
    return vso;
}