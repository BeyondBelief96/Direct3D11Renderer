// =============================================================================
// Simple Solid Color Pixel Shader
// =============================================================================
// Outputs a constant solid color - no lighting calculations.
// Perfect for outline effects and simple solid color rendering.
// =============================================================================

// Material properties constant buffer
cbuffer SolidColorMaterial : register(b1)
{
    float4 color;  // RGBA color to output
};

// Main pixel shader entry point
float4 main() : SV_TARGET
{
    return color;
}