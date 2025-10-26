#include "Common/LightingCommon.hlsli"
#include "Common/NormalMapping.hlsli"
#include "Common/CommonStructures.hlsli"

cbuffer MaterialProperties : register(b1)
{
    float3 specularColor;
    float specularWeight;
    float specularGloss;
    bool useNormalMap;
    float normalMapWeight;
};

Texture2D diffuseTexture : register(t0);
Texture2D normalMap : register(t2);
SamplerState textureSampler;

float4 main(float3 viewSpacePosition : Position, float3 viewSpaceNormal : Normal, float3 viewSpaceTangent : Tangent, float3 viewSpaceBitangent : Bitangent, float2 textureCoords : TexCoord) : SV_TARGET
{
     // === NORMAL CALCULATION (WITH NORMAL MAPPING) ===
    float3 surfaceNormal = viewSpaceNormal;
    
    if (useNormalMap)
    {
        // Normalize TBN vectors for consistent results
        float3 normalizedTangent, normalizedBitangent, normalizedNormal;
        NormalizeTBNVectors(viewSpaceTangent, viewSpaceBitangent, viewSpaceNormal,
                           normalizedTangent, normalizedBitangent, normalizedNormal);
        
        // Apply normal mapping using composable function
        surfaceNormal = ApplyNormalMapping(
            normalMap,
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
        specularGloss,
        specularWeight,
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