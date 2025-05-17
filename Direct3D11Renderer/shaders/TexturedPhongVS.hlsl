cbuffer TransformConstantBuffer
{
    matrix modelView;
    matrix modelViewProj;
};

struct VSOut
{
    float3 vertexViewPos : Position;
    float3 normal : Normal;
    float2 texCoord : TexCoord;
    float4 pos : SV_Position;
};

VSOut main(float3 pos : Position, float3 normal : Normal, float2 texCoord : TexCoord)
{
    VSOut output;
    output.vertexViewPos = mul(float4(pos, 1.0f), modelView).xyz;
    output.normal = mul(normal, (float3x3) modelView);
    output.pos = mul(float4(pos, 1.0f), modelViewProj);
    output.texCoord = texCoord;
    return output;

}