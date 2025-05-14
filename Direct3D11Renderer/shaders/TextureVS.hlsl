
cbuffer cbuf
{
    matrix transform;
};

struct VSOut
{
    float2 texCoord : TEXCOORD;
    float4 position : SV_Position;
};

VSOut main(float3 pos : POSITION, float2 texCoord : TEXCOORD)
{
    VSOut vso;
    vso.position = mul(float4(pos, 1.0f), transform);
    vso.texCoord = texCoord;
    return vso;
}