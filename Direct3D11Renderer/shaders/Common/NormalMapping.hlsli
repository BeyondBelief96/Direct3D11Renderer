// =============================================================================
// Normal Mapping Common Functions
// =============================================================================
// Shared normal mapping operations for transforming normals between spaces.
// Provides utility functions for tangent-space normal mapping.
// =============================================================================

#ifndef NORMAL_MAPPING_HLSLI
#define NORMAL_MAPPING_HLSLI

// =============================================================================
// NORMAL MAPPING FUNCTIONS
// =============================================================================

/// <summary>
/// Samples and expands a normal from a tangent-space normal map.
/// Converts from [0,1] texture range to [-1,1] normal range and handles coordinate system differences.
/// </summary>
/// <param name="normalTexture">Normal map texture to sample from</param>
/// <param name="textureSampler">Texture sampler state</param>
/// <param name="texCoords">UV coordinates for sampling</param>
/// <returns>Tangent-space normal vector in [-1,1] range</returns>
float3 SampleTangentSpaceNormal(
    Texture2D normalTexture,
    SamplerState textureSampler,
    in float2 texCoords)
{
    // Sample normal from texture (stored in [0,1] range)
    const float3 normalSample = normalTexture.Sample(textureSampler, texCoords).xyz;
    
    // Convert from [0,1] to [-1,1] range
    float3 tangentSpaceNormal = normalSample * 2.0f - 1.0f;
    
    // Flip Y component to account for DirectX texture coordinate system
    // (Some normal maps are authored for OpenGL which has flipped Y)
    tangentSpaceNormal.y = -tangentSpaceNormal.y;
    
    return tangentSpaceNormal;
}

/// <summary>
/// Transforms a tangent-space normal to view space using pre-normalized TBN vectors.
/// </summary>
/// <param name="tangentSpaceNormal">Normal vector in tangent space [-1,1]</param>
/// <param name="normalizedTangent">Normalized tangent vector in view space</param>
/// <param name="normalizedBitangent">Normalized bitangent vector in view space</param>
/// <param name="normalizedNormal">Normalized surface normal in view space</param>
/// <returns>Normalized normal vector in view space</returns>
float3 TransformTangentToViewSpace(
    in float3 tangentSpaceNormal,
    in float3 normalizedTangent,
    in float3 normalizedBitangent,
    in float3 normalizedNormal)
{
    // Build the Tangent-Bitangent-Normal (TBN) matrix
    // This transforms vectors from tangent space to view space
    const float3x3 tbnMatrix = float3x3(
        normalizedTangent,
        normalizedBitangent,
        normalizedNormal
    );
    
    // Transform and normalize the result
    return normalize(mul(tangentSpaceNormal, tbnMatrix));
}

/// <summary>
/// Complete normal mapping pipeline: sample texture and transform to view space.
/// Expects pre-normalized TBN vectors for optimal performance.
/// </summary>
/// <param name="normalTexture">Normal map texture</param>
/// <param name="textureSampler">Texture sampler state</param>
/// <param name="texCoords">UV coordinates</param>
/// <param name="normalizedTangent">Normalized tangent vector in view space</param>
/// <param name="normalizedBitangent">Normalized bitangent vector in view space</param>
/// <param name="normalizedNormal">Normalized surface normal in view space</param>
/// <returns>Final normal vector in view space</returns>
float3 ApplyNormalMapping(
    Texture2D normalTexture,
    SamplerState textureSampler,
    in float2 texCoords,
    in float3 normalizedTangent,
    in float3 normalizedBitangent,
    in float3 normalizedNormal)
{
    const float3 tangentSpaceNormal = SampleTangentSpaceNormal(normalTexture, textureSampler, texCoords);
    return TransformTangentToViewSpace(tangentSpaceNormal, normalizedTangent, normalizedBitangent, normalizedNormal);
}

/// <summary>
/// Normalizes TBN vectors for consistent normal mapping results.
/// Call this once per pixel before using other normal mapping functions.
/// </summary>
/// <param name="tangent">Input tangent vector from vertex shader</param>
/// <param name="bitangent">Input bitangent vector from vertex shader</param>
/// <param name="normal">Input normal vector from vertex shader</param>
/// <param name="normalizedTangent">Output: normalized tangent</param>
/// <param name="normalizedBitangent">Output: normalized bitangent</param>
/// <param name="normalizedNormal">Output: normalized normal</param>
void NormalizeTBNVectors(
    in float3 tangent,
    in float3 bitangent,
    in float3 normal,
    out float3 normalizedTangent,
    out float3 normalizedBitangent,
    out float3 normalizedNormal)
{
    normalizedTangent = normalize(tangent);
    normalizedBitangent = normalize(bitangent);
    normalizedNormal = normalize(normal);
}

#endif // NORMAL_MAPPING_HLSLI