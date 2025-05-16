cbuffer TransformCBuffer
{
    matrix model;
    matrix modelViewProj;
};

struct VSOut
{
    float3 worldPos : POSITION;
    float3 normal : NORMAL;
    float4 position : SV_Position;
};

VSOut main( float3 pos : POSITION, float3 normal : NORMAL )
{
    VSOut output;
    output.worldPos = (float3) mul(float4(pos, 1.0f), model);
    output.normal = mul(normal, (float3x3) model);
    output.position = mul(float4(pos, 1.0f), modelViewProj);
    return output;

}