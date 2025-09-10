cbuffer TransformBuffer : register(b0)
{
    matrix modelView;
    matrix modelViewProjection;
}

struct VSOut
{
    float3 fragPosViewSpace : Position;
    float3 normal : Normal;
    float4 position : SV_Position;
};

VSOut main( float3 pos : Position, float3 normal : Normal )
{
    VSOut output;
    output.fragPosViewSpace = mul(float4(pos, 1.0f), modelView).xyz;
    output.normal = normalize(mul(float4(normal, 0.0f), modelView)).xyz;
    output.position = mul(float4(pos, 1.0f), modelViewProjection);
    return output;

}