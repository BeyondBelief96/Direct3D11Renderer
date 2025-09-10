// =============================================================================
// Blinn-Phong Solid Color Vertex Shader
// =============================================================================
// Transforms vertices for solid-colored models (no textures) using Blinn-Phong shading.
// Outputs view-space position and normal for lighting calculations.
// =============================================================================

// Transform matrices constant buffer
cbuffer TransformMatrices : register(b0)
{
    matrix modelViewMatrix;       // Transform from model space to view (camera) space
    matrix modelViewProjMatrix;   // Transform from model space to clip space for rasterization
};

// Vertex shader output structure
struct VertexOutput
{
    float3 viewSpacePosition : Position;     // Fragment position in view space (for lighting calculations)
    float3 viewSpaceNormal   : Normal;       // Surface normal in view space (for lighting calculations)
    float4 clipSpacePosition : SV_Position;  // Final transformed position for rasterization
};

// Main vertex shader entry point
VertexOutput main(float3 modelPosition : Position, float3 modelNormal : Normal)
{
    VertexOutput output;
    
    // Transform vertex position from model space to view space
    // This is needed for per-pixel lighting calculations in the pixel shader
    output.viewSpacePosition = mul(float4(modelPosition, 1.0f), modelViewMatrix).xyz;
    
    // Transform normal from model space to view space
    // Use only the 3x3 rotation part of the matrix (ignore translation)
    // Normalize to ensure unit length after transformation
    output.viewSpaceNormal = normalize(mul(float4(modelNormal, 0.0f), modelViewMatrix).xyz);
    
    // Transform position to clip space for GPU rasterization
    output.clipSpacePosition = mul(float4(modelPosition, 1.0f), modelViewProjMatrix);
    
    return output;
}