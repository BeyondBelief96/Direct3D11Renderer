#include "Common/CommonStructures.hlsli"

NormalMappedVertexOutput main(float3 pos : Position, float3 n : Normal, float2 texCoord : TexCoord, float3 tangent : Tangent, float3 bitangent : Bitangent)
{
    NormalMappedVertexOutput output;
    output.viewSpacePosition = (float3) mul(float4(pos, 1.0f), modelViewMatrix);
    output.viewSpaceNormal = mul(n, (float3x3) modelViewMatrix);
    output.viewSpaceTangent = mul(tangent, (float3x3) modelViewMatrix);
    output.viewSpaceBitangent = mul(bitangent, (float3x3) modelViewMatrix);
    output.clipSpacePosition = mul(float4(pos, 1.0f), modelViewProjMatrix);
    output.textureCoords = texCoord;
    return output;
}