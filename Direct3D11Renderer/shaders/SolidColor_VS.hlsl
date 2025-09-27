// =============================================================================
// Simple Solid Color Vertex Shader
// =============================================================================
// Basic vertex transformation for solid color rendering.
// Transforms vertices to clip space without any lighting calculations.
// =============================================================================

#include "Common/CommonStructures.hlsli"

// Vertex input structure
struct VSInput
{
    float3 pos : Position;
    float3 normal : Normal;
};

// Vertex output structure (minimal for solid color)
struct VSOutput
{
    float4 pos : SV_POSITION;
};

// Main vertex shader entry point
VSOutput main(VSInput input)
{
    VSOutput output;

    // Transform vertex position to clip space
    output.pos = mul(float4(input.pos, 1.0f), modelViewProjMatrix);

    return output;
}