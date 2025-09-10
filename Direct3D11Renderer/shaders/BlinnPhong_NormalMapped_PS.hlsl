// =============================================================================
// Blinn-Phong Normal Mapped Pixel Shader
// =============================================================================
// Implements Blinn-Phong lighting model with normal mapping support.
// Uses composable functions for lighting and normal mapping operations.
// =============================================================================

#include "Common/CommonStructures.hlsli"
#include "Common/LightingCommon.hlsli"
#include "Common/NormalMapping.hlsli"

// Material properties constant buffer
cbuffer NormalMappedMaterialProperties : register(b1)
{
    float specularReflectance;     // How much specular light the material reflects (0-1)
    float specularShininess;       // Controls specular highlight size (higher = smaller highlight)
    bool  normalMappingEnabled;    // Toggle for normal mapping calculations
    float materialPadding;         // Padding for 16-byte alignment requirement
};

// Texture resources
Texture2D diffuseTexture : register(t0);    // Main color/albedo texture
Texture2D normalTexture  : register(t2);    // Normal map texture (tangent space)
SamplerState textureSampler : register(s0); // Texture sampling settings

// Main pixel shader entry point
float4 main(
    in float3 viewSpacePosition : Position, 
    in float3 viewSpaceNormal   : Normal, 
    in float3 viewSpaceTangent  : Tangent, 
    in float3 viewSpaceBitangent: Bitangent, 
    in float2 textureCoords     : TexCoord) : SV_Target
{
    // === NORMAL CALCULATION (WITH NORMAL MAPPING) ===
    float3 surfaceNormal = viewSpaceNormal;
    
    if (normalMappingEnabled)
    {
        // Normalize TBN vectors for consistent results
        float3 normalizedTangent, normalizedBitangent, normalizedNormal;
        NormalizeTBNVectors(viewSpaceTangent, viewSpaceBitangent, viewSpaceNormal,
                           normalizedTangent, normalizedBitangent, normalizedNormal);
        
        // Apply normal mapping using composable function
        surfaceNormal = ApplyNormalMapping(
            normalTexture,
            textureSampler,
            textureCoords,
            normalizedTangent,
            normalizedBitangent,
            normalizedNormal);
    }
    else
    {
        // Just normalize the surface normal if no normal mapping
        surfaceNormal = normalize(viewSpaceNormal);
    }
    
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
        surfaceNormal,
        lightDirection,
        diffuseLightColor,
        diffuseLightIntensity,
        attenuation);
    
    // === SPECULAR LIGHTING ===
    const float3 specularComponent = CalculateBlinnPhongSpecular(
        surfaceNormal,
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
    const float3 finalColor = saturate((ambientComponent + diffuseComponent) * textureColor.rgb + specularComponent);
    
    return float4(finalColor, 1.0f);
}