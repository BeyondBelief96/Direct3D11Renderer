// =============================================================================
// Blinn-Phong Diffuse Pixel Shader
// =============================================================================
// Implements Blinn-Phong lighting model with diffuse texture support.
// Uses composable lighting functions for maintainable code.
// =============================================================================

#include "Common/CommonStructures.hlsli"
#include "Common/LightingCommon.hlsli"

// Material properties constant buffer
cbuffer MaterialProperties : register(b1)
{
    float specularReflectance;     // How much specular light the material reflects (0-1)
    float specularShininess;       // Controls specular highlight size (higher = smaller highlight)
    float2 materialPadding;        // Padding for 16-byte alignment requirement
};

// Texture resources
Texture2D diffuseTexture : register(t0);    // Main color/albedo texture
SamplerState textureSampler : register(s0); // Texture sampling settings

// Main pixel shader entry point
float4 main(
    in float3 viewSpacePosition : Position, 
    in float3 viewSpaceNormal : Normal, 
    in float2 textureCoords : TexCoord) : SV_TARGET
{
    // === RENORMALIZATION ===
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
    
    // === DIFFUSE LIGHTING ===
    const float3 diffuseComponent = CalculateDiffuseLighting(
        normal,
        lightDirection,
        diffuseLightColor,
        diffuseLightIntensity,
        attenuation);
    
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
    
    // === AMBIENT LIGHTING ===
    const float3 ambientComponent = CalculateAmbientLighting(ambientLightColor, float3(1.0f, 1.0f, 1.0f));
    
    // === FINAL COLOR COMBINATION ===
    // Sample the diffuse texture
    const float4 textureColor = diffuseTexture.Sample(textureSampler, textureCoords);
    
    // Combine all lighting components:
    // - Ambient + Diffuse are modulated by texture color
    // - Specular is added on top (represents surface reflection, not texture color)
    const float3 finalColor = saturate((diffuseComponent + ambientComponent) * textureColor.rgb + specularComponent);
    
    return float4(finalColor, 1.0f);
}