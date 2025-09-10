// =============================================================================
// Blinn-Phong Normal Mapped Vertex Shader
// =============================================================================
// Transforms vertices for normal-mapped models using Blinn-Phong shading.
// Outputs tangent-space basis vectors for normal mapping calculations.
// =============================================================================

#include "Common/CommonStructures.hlsli"

// Main vertex shader entry point
NormalMappedVertexOutput main(
    in float3 modelPosition  : Position, 
    in float3 modelNormal    : Normal, 
    in float3 modelTangent   : Tangent, 
    in float3 modelBitangent : Bitangent, 
    in float2 texCoords      : TexCoord)
{
    NormalMappedVertexOutput output;
    
    // Transform vertex position from model space to view space
    // This is needed for per-pixel lighting calculations in the pixel shader
    output.viewSpacePosition = mul(float4(modelPosition, 1.0f), modelViewMatrix).xyz;
    
    // Transform the tangent-space basis vectors (TBN matrix components) to view space
    // These vectors define the local coordinate system at each vertex for normal mapping
    // Use only the 3x3 rotation part of the matrix (ignore translation)
    output.viewSpaceNormal    = mul(modelNormal,    (float3x3)modelViewMatrix);
    output.viewSpaceTangent   = mul(modelTangent,   (float3x3)modelViewMatrix);
    output.viewSpaceBitangent = mul(modelBitangent, (float3x3)modelViewMatrix);
    
    // Pass texture coordinates through unchanged
    output.textureCoords = texCoords;
    
    // Transform position to clip space for GPU rasterization
    output.clipSpacePosition = mul(float4(modelPosition, 1.0f), modelViewProjMatrix);
    
    return output;
}