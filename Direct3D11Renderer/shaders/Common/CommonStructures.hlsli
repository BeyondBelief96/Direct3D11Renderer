// =============================================================================
// Common Shader Structures and Constants
// =============================================================================
// Shared constant buffer layouts and data structures used across shaders.
// Ensures consistent data layout and register assignments.
// =============================================================================

#ifndef COMMON_STRUCTURES_HLSLI
#define COMMON_STRUCTURES_HLSLI

// =============================================================================
// TRANSFORM CONSTANT BUFFER (VERTEX SHADERS)
// =============================================================================

/// <summary>
/// Transform matrices for vertex transformations.
/// Register b0 - Used by vertex shaders.
/// </summary>
cbuffer TransformMatrices : register(b0)
{
    matrix modelViewMatrix;       // Transform from model space to view (camera) space
    matrix modelViewProjMatrix;   // Transform from model space to clip space for rasterization
};

// =============================================================================
// LIGHTING CONSTANT BUFFER (PIXEL SHADERS) 
// =============================================================================

/// <summary>
/// Point light properties for lighting calculations.
/// Register b0 - Used by pixel shaders.
/// </summary>
cbuffer PointLightProperties : register(b0)
{
    float3 lightPositionViewSpace; // Light position in view space coordinates
    float3 ambientLightColor;      // Base ambient lighting color
    float3 diffuseLightColor;      // Direct light color for diffuse calculations
    float  diffuseLightIntensity;  // Brightness multiplier for diffuse light
    float  attenuationConstant;    // Distance attenuation constant factor (usually 1.0)
    float  attenuationLinear;      // Distance attenuation linear factor
    float  attenuationQuadratic;   // Distance attenuation quadratic factor
};

// =============================================================================
// VERTEX SHADER OUTPUT STRUCTURES
// =============================================================================

/// <summary>
/// Basic vertex output for textured models.
/// </summary>
struct BasicVertexOutput
{
    float3 viewSpacePosition : Position;     // Fragment position in view space (for lighting calculations)
    float3 viewSpaceNormal   : Normal;       // Surface normal in view space (for lighting calculations)
    float2 textureCoords     : TexCoord;     // UV coordinates for texture sampling
    float4 clipSpacePosition : SV_Position;  // Final transformed position for rasterization
};

/// <summary>
/// Vertex output for solid color models (no textures).
/// </summary>
struct SolidVertexOutput
{
    float3 viewSpacePosition : Position;     // Fragment position in view space (for lighting calculations)
    float3 viewSpaceNormal   : Normal;       // Surface normal in view space (for lighting calculations)
    float4 clipSpacePosition : SV_Position;  // Final transformed position for rasterization
};

/// <summary>
/// Vertex output for normal mapped models.
/// </summary>
struct NormalMappedVertexOutput
{
    float3 viewSpacePosition : Position;     // Fragment position in view space (for lighting calculations)
    float3 viewSpaceNormal   : Normal;       // Surface normal in view space (for lighting calculations)
    float3 viewSpaceTangent  : Tangent;      // Tangent vector in view space (for normal mapping)
    float3 viewSpaceBitangent: Bitangent;    // Bitangent vector in view space (for normal mapping)
    float2 textureCoords     : TexCoord;     // UV coordinates for texture sampling
    float4 clipSpacePosition : SV_Position;  // Final transformed position for rasterization
};

#endif // COMMON_STRUCTURES_HLSLI