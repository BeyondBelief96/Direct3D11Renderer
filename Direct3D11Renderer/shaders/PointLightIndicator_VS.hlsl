// =============================================================================
// Point Light Indicator Vertex Shader
// =============================================================================
// Simple vertex shader for rendering point light visualization spheres.
// Only transforms vertex positions without any lighting calculations.
// =============================================================================

// Transform matrices constant buffer
cbuffer TransformMatrices : register(b0)
{
    matrix modelViewMatrix;       // Transform from model space to view (camera) space
    matrix modelViewProjMatrix;   // Transform from model space to clip space for rasterization
};

// Main vertex shader entry point
// Input: vertex position in model space
// Output: transformed position for rasterization
float4 main(float3 modelPosition : Position) : SV_Position
{
    // Transform vertex position directly from model space to clip space
    // No need for intermediate calculations since this is just a simple indicator geometry
    return mul(float4(modelPosition, 1.0f), modelViewProjMatrix);
}