// =============================================================================
// Blinn-Phong Diffuse Vertex Shader
// =============================================================================
// Transforms vertices for textured models using Blinn-Phong shading.
// Outputs view-space position, normal, and texture coordinates.
// =============================================================================

#include "Common/CommonStructures.hlsli"

// Main vertex shader entry point
BasicVertexOutput main(
    in float3 modelPosition : Position, 
    in float3 modelNormal : Normal, 
    in float2 texCoords : TexCoord)
{
    BasicVertexOutput output;
    
    // Transform vertex position from model space to view space
    // This is needed for per-pixel lighting calculations in the pixel shader
    output.viewSpacePosition = mul(float4(modelPosition, 1.0f), modelViewMatrix).xyz;
    
    // Transform normal from model space to view space
    // Use only the 3x3 rotation part of the matrix (ignore translation)
    // Normalize to ensure unit length after transformation
    output.viewSpaceNormal = normalize(mul(modelNormal, (float3x3)modelViewMatrix));
    
    // Pass texture coordinates through unchanged
    output.textureCoords = texCoords;
    
    // Transform position to clip space for GPU rasterization
    output.clipSpacePosition = mul(float4(modelPosition, 1.0f), modelViewProjMatrix);
    
    return output;
}