// =============================================================================
// Lighting Common Functions
// =============================================================================
// Shared lighting calculations and utility functions for all shaders.
// Provides composable functions for Blinn-Phong lighting model components.
// =============================================================================

#ifndef LIGHTING_COMMON_HLSLI
#define LIGHTING_COMMON_HLSLI

// =============================================================================
// LIGHT ATTENUATION FUNCTIONS
// =============================================================================

/// <summary>
/// Calculates distance-based light attenuation using quadratic falloff.
/// Formula: 1 / (constant + linear*distance + quadratic*distance²)
/// </summary>
/// <param name="distance">Distance from light to fragment</param>
/// <param name="constantFactor">Constant attenuation factor (usually 1.0)</param>
/// <param name="linearFactor">Linear attenuation factor</param>
/// <param name="quadraticFactor">Quadratic attenuation factor</param>
/// <returns>Attenuation multiplier (0-1)</returns>
float CalculateDistanceAttenuation(
    in float distance,
    in float constantFactor,
    in float linearFactor,
    in float quadraticFactor)
{
    return 1.0f / (constantFactor + linearFactor * distance + quadraticFactor * (distance * distance));
}

// =============================================================================
// DIFFUSE LIGHTING FUNCTIONS
// =============================================================================

/// <summary>
/// Calculates Lambertian diffuse lighting component.
/// Uses dot product between surface normal and light direction.
/// </summary>
/// <param name="normal">Normalized surface normal in view space</param>
/// <param name="lightDirection">Normalized direction to light source</param>
/// <param name="lightColor">RGB color of the light</param>
/// <param name="lightIntensity">Brightness multiplier</param>
/// <param name="attenuation">Distance/angle attenuation factor</param>
/// <returns>Diffuse lighting contribution (RGB)</returns>
float3 CalculateDiffuseLighting(
    in float3 normal,
    in float3 lightDirection,
    in float3 lightColor,
    in float lightIntensity,
    in float attenuation)
{
    const float nDotL = max(0.0f, dot(normal, lightDirection));
    return lightColor * lightIntensity * attenuation * nDotL;
}

// =============================================================================
// SPECULAR LIGHTING FUNCTIONS
// =============================================================================

/// <summary>
/// Calculates Blinn-Phong specular lighting component using reflection vector.
/// </summary>
/// <param name="normal">Normalized surface normal in view space</param>
/// <param name="lightDirection">Normalized direction to light source</param>
/// <param name="viewDirection">Normalized direction to camera (usually -normalize(viewPos))</param>
/// <param name="lightColor">RGB color of the light</param>
/// <param name="lightIntensity">Brightness multiplier</param>
/// <param name="specularReflectance">Material specular reflectance (0-1)</param>
/// <param name="shininess">Specular power/shininess (higher = tighter highlights)</param>
/// <param name="attenuation">Distance/angle attenuation factor</param>
/// <returns>Specular lighting contribution (RGB)</returns>
float3 CalculateBlinnPhongSpecular(
    in float3 normal,
    in float3 lightDirection,
    in float3 viewDirection,
    in float3 lightColor,
    in float lightIntensity,
    in float specularReflectance,
    in float shininess,
    in float attenuation)
{
    const float3 reflectionVector = reflect(-lightDirection, normal);
    const float specularFactor = pow(max(0.0f, dot(viewDirection, reflectionVector)), shininess);
    return lightColor * lightIntensity * specularReflectance * attenuation * specularFactor;
}

/// <summary>
/// Calculates specular lighting with per-pixel specular color (from specular map).
/// </summary>
/// <param name="normal">Normalized surface normal in view space</param>
/// <param name="lightDirection">Normalized direction to light source</param>
/// <param name="viewDirection">Normalized direction to camera</param>
/// <param name="lightColor">RGB color of the light</param>
/// <param name="lightIntensity">Brightness multiplier</param>
/// <param name="specularColor">Per-pixel specular color (from texture)</param>
/// <param name="shininess">Specular power/shininess</param>
/// <param name="attenuation">Distance/angle attenuation factor</param>
/// <returns>Specular lighting contribution (RGB)</returns>
float3 CalculateBlinnPhongSpecularMapped(
    in float3 normal,
    in float3 lightDirection,
    in float3 viewDirection,
    in float3 lightColor,
    in float lightIntensity,
    in float3 specularColor,
    in float shininess,
    in float attenuation)
{
    const float3 reflectionVector = reflect(-lightDirection, normal);
    const float specularFactor = pow(max(0.0f, dot(viewDirection, reflectionVector)), shininess);
    return lightColor * lightIntensity * attenuation * specularFactor * specularColor;
}

// =============================================================================
// AMBIENT LIGHTING FUNCTIONS
// =============================================================================

/// <summary>
/// Calculates ambient lighting component.
/// </summary>
/// <param name="ambientColor">Ambient light color</param>
/// <param name="materialColor">Material base color</param>
/// <param name="attenuation">Optional attenuation factor</param>
/// <returns>Ambient lighting contribution (RGB)</returns>
float3 CalculateAmbientLighting(
    in float3 ambientColor,
    in float3 materialColor,
    in float attenuation = 1.0f)
{
    return ambientColor * materialColor * attenuation;
}

// =============================================================================
// VECTOR UTILITY FUNCTIONS
// =============================================================================

/// <summary>
/// Calculates and normalizes the vector from fragment to light.
/// </summary>
/// <param name="lightPosition">Light position in view space</param>
/// <param name="fragmentPosition">Fragment position in view space</param>
/// <param name="lightDirection">Output: normalized direction to light</param>
/// <param name="distance">Output: distance to light</param>
void CalculateLightVector(
    in float3 lightPosition,
    in float3 fragmentPosition,
    out float3 lightDirection,
    out float distance)
{
    const float3 lightVector = lightPosition - fragmentPosition;
    distance = length(lightVector);
    lightDirection = lightVector / distance;
}

/// <summary>
/// Calculates view direction for specular calculations.
/// In view space, camera is at origin, so view direction is -normalize(fragmentPos).
/// </summary>
/// <param name="fragmentPosition">Fragment position in view space</param>
/// <returns>Normalized view direction</returns>
float3 CalculateViewDirection(in float3 fragmentPosition)
{
    return normalize(-fragmentPosition);
}

// =============================================================================
// GLOSS/SHININESS UTILITY FUNCTIONS
// =============================================================================

/// <summary>
/// Converts gloss value (0-1) to specular power using exponential mapping.
/// Provides range from 1 to ~8192 for good highlight control.
/// </summary>
/// <param name="glossValue">Gloss value from texture alpha channel (0-1)</param>
/// <returns>Specular power for lighting calculations</returns>
float GlossToSpecularPower(in float glossValue)
{
    return pow(2.0f, glossValue * 13.0f);
}

#endif // LIGHTING_COMMON_HLSLI