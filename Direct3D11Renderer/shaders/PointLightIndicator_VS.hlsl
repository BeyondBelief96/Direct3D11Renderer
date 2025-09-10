// =============================================================================
// Point Light Indicator Vertex Shader
// =============================================================================
// Simple vertex shader for rendering point light visualization spheres.
// Only transforms vertex positions without any lighting calculations.
// =============================================================================

#include "Common/CommonStructures.hlsli"

// Main vertex shader entry point
// Input: vertex position in model space
// Output: transformed position for rasterization
float4 main(in float3 modelPosition : Position) : SV_Position
{
    // Transform vertex position directly from model space to clip space
    // No need for intermediate calculations since this is just a simple indicator geometry
    return mul(float4(modelPosition, 1.0f), modelViewProjMatrix);
}