// =============================================================================
// Blinn-Phong Solid Color Pixel Shader
// =============================================================================
// Implements Blinn-Phong lighting model with solid material color (no textures).
// Uses composable lighting functions for maintainable code.
// =============================================================================

#include "Common/CommonStructures.hlsli"
#include "Common/LightingCommon.hlsli"

// Material properties constant buffer
cbuffer SolidMaterialProperties : register(b1)
{
    float4 materialDiffuseColor;   // Base color of the material (RGB + Alpha)
    float  specularReflectance;    // How much specular light the material reflects (0-1)
    float  specularShininess;      // Controls specular highlight size (higher = smaller highlight)
    float  materialPadding;        // Padding for 16-byte alignment requirement
};

// Main pixel shader entry point
float4 main(
    in float3 viewSpacePosition : Position, 
    in float3 viewSpaceNormal : Normal) : SV_TARGET
{
    // === RENORMALIZE SURFACE NORMAL ===
    // Ensure surface normal is normalized after interpolation
    const float3 normal = normalize(viewSpaceNormal);
    
    // === LIGHT VECTOR CALCULATIONS ===
    float3 lightDirection;
    float distanceToLight;
    CalculateLightVector(lightPositionViewSpace, viewSpacePosition, lightDirection, distanceToLight);
    
    // === VIEW DIRECTION ===
    const float3 viewDirection = CalculateViewDirection(viewSpacePosition);
    
    // === DISTANCE ATTENUATION ===
    const float attenuation = CalculateDistanceAttenuation(
        distanceToLight,
        attenuationConstant,
        attenuationLinear,
        attenuationQuadratic);
    
    // === AMBIENT LIGHTING ===
    const float3 ambientComponent = CalculateAmbientLighting(
        ambientLightColor, 
        materialDiffuseColor.rgb, 
        attenuation);
    
    // === DIFFUSE LIGHTING ===
    const float3 diffuseComponent = CalculateDiffuseLighting(
        normal,
        lightDirection,
        diffuseLightColor,
        diffuseLightIntensity,
        attenuation) * materialDiffuseColor.rgb;
    
    // === SPECULAR LIGHTING ===
    const float3 specularComponent = CalculateBlinnPhongSpecular(
        normal,
        lightDirection,
        viewDirection,
        diffuseLightColor,
        diffuseLightIntensity,
        specularReflectance,
        specularShininess,
        attenuation);
    
    // === FINAL COLOR COMBINATION ===
    // Combine all lighting components and saturate to prevent over-bright colors
    const float3 finalColor = saturate(diffuseComponent + ambientComponent + specularComponent);
    
    return float4(finalColor, 1.0f);
}