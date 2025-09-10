cbuffer TransformBuffer : register(b0)
{
    matrix modelView;
    matrix modelViewProjection;
};

struct VSOut
{
    float3 viewSpacePosition : Position;
    float3 normal : Normal;
    float3 tangent : Tangent;
    float3 bitangent : Bitangent;
    float2 texCoord : TexCoord;
    float4 pos : SV_Position;
};

VSOut main(float3 position : Position, float3 normal : Normal, float3 tangent : Tangent, float3 bitangent : Bitangent, float2 texCoord : TexCoord)
{
    VSOut output;
    output.viewSpacePosition = (float3) mul(float4(position, 1.0f), modelView);
    output.normal = mul(normal, (float3x3) modelView);
    output.tangent = mul(tangent, (float3x3) modelView);
    output.bitangent = mul(bitangent, (float3x3) modelView);
    output.pos = mul(float4(position, 1.0f), modelViewProjection);
    output.texCoord = texCoord;
    return output;
};