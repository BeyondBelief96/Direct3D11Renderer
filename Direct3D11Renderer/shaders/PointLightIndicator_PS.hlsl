// =============================================================================
// Point Light Indicator Pixel Shader
// =============================================================================
// Simple pixel shader that outputs a solid color for point light visualization.
// Used to render small spheres that show where point lights are positioned.
// =============================================================================

// Light visualization properties constant buffer
cbuffer LightIndicatorProperties : register(b1)
{
    float4 lightIndicatorColor;    // RGBA color for the light indicator sphere
};

// Main pixel shader entry point
// Output: solid color for light visualization
float4 main() : SV_Target
{
    // Simply return the configured indicator color
    // No lighting calculations needed - this is just a visual aid
    return lightIndicatorColor;
}