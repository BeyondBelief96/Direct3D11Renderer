// =============================================================================
// Blinn-Phong Specular + Normal Mapped Pixel Shader
// =============================================================================
// Implements Blinn-Phong lighting with both normal mapping and specular mapping.
// Uses composable functions for lighting, normal mapping, and gloss calculations.
// =============================================================================

#include "Common/CommonStructures.hlsli"
#include "Common/LightingCommon.hlsli"
#include "Common/NormalMapping.hlsli"

// Material properties constant buffer
cbuffer SpecularNormalMappedMaterialProperties : register(b1)
{
    bool  hasGlossInAlphaChannel;  // Whether specular map's alpha contains gloss information
    bool  normalMappingEnabled;    // Toggle for normal mapping calculations
    float baseSpecularShininess;   // Default specular power when no gloss map is used
    float materialPadding;         // Padding for 16-byte alignment requirement
};

// Texture resources
Texture2D diffuseTexture  : register(t0);    // Main color/albedo texture
Texture2D specularTexture : register(t1);    // Specular reflection map (RGB) + optional gloss (A)
Texture2D normalTexture   : register(t2);    // Normal map texture (tangent space)
SamplerState textureSampler : register(s0);  // Texture sampling settings

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
    
    // === SPECULAR MAPPING AND GLOSS CALCULATION ===
    // Sample the specular map to get per-pixel specular properties
    const float4 specularSample = specularTexture.Sample(textureSampler, textureCoords);
    const float3 specularReflectionColor = specularSample.rgb;
    
    // Determine specular power (shininess) - either from gloss map or constant
    float specularPower = baseSpecularShininess;
    if (hasGlossInAlphaChannel)
    {
        // Convert gloss value to specular power using utility function
        specularPower = GlossToSpecularPower(specularSample.a);
    }
    
    // === SPECULAR LIGHTING WITH MAPPED PROPERTIES ===
    const float3 specularComponent = CalculateBlinnPhongSpecularMapped(
        surfaceNormal,
        lightDirection,
        viewDirection,
        diffuseLightColor,
        diffuseLightIntensity,
        specularReflectionColor,
        specularPower,
        attenuation);
    
    // === AMBIENT LIGHTING ===
    const float3 ambientComponent = CalculateAmbientLighting(ambientLightColor, float3(1.0f, 1.0f, 1.0f));
    
    // === FINAL COLOR COMBINATION ===
    // Sample the diffuse texture
    const float4 textureColor = diffuseTexture.Sample(textureSampler, textureCoords);
    
    // Combine all lighting components:
    // - Ambient + Diffuse are modulated by texture color
    // - Specular uses the specular map color (represents different material reflectance)
    const float3 finalColor = saturate((ambientComponent + diffuseComponent) * textureColor.rgb + specularComponent);
    
    return float4(finalColor, 1.0f);
}