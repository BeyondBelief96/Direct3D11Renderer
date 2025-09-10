// =============================================================================
// Blinn-Phong Normal Mapped Pixel Shader
// =============================================================================
// Implements Blinn-Phong lighting model with normal mapping support.
// Uses tangent-space normal maps to add surface detail without additional geometry.
// =============================================================================

// Point light properties constant buffer
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

// Material properties constant buffer
cbuffer MaterialProperties : register(b1)
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

float3 MapNormalTangentSpaceToViewSpace(
    const float3 viewSpaceTangent,
    const float3 viewSpaceBitangent,
    const float3 viewSpaceNormal,
    const float2 textureCoordinate,
    Texture2D normalMap,
    SamplerState splr)
{
    // Build the Tangent-Bitangent-Normal (TBN) matrix
        // This transforms vectors from tangent space to view space
    const float3x3 tangentToViewSpaceMatrix = float3x3(
            normalize(viewSpaceTangent),
            normalize(viewSpaceBitangent),
            normalize(viewSpaceNormal)
        );
        
        // Sample normal from normal map (stored in tangent space)
        // Normal maps store normals in [0,1] range, so convert to [-1,1]
    const float3 tangentSpaceNormal = normalTexture.Sample(textureSampler, textureCoordinate).xyz;
    
    float3 expandedNormal = tangentSpaceNormal * 2.0f - 1.0f;
        
        // Flip Y component to account for DirectX texture coordinate system
        // (Some normal maps are authored for OpenGL which has flipped Y)
    expandedNormal.y = -expandedNormal.y;
    
    // Transform the tangent-space normal to view space using TBN matrix
    return normalize(mul(expandedNormal, tangentToViewSpaceMatrix));
}

// Main pixel shader entry point
float4 main(float3 viewSpacePosition : Position, 
            float3 viewSpaceNormal   : Normal, 
            float3 viewSpaceTangent  : Tangent, 
            float3 viewSpaceBitangent: Bitangent, 
            float2 textureCoords     : TexCoord) : SV_Target
{
    // === NORMAL CALCULATION (WITH NORMAL MAPPING) ===
    float3 surfaceNormal = viewSpaceNormal;
    
    if (normalMappingEnabled)
    {
        // Transform the tangent-space normal to view space using TBN matrix
        surfaceNormal = MapNormalTangentSpaceToViewSpace(
            viewSpaceTangent,
            viewSpaceBitangent,
            viewSpaceNormal,
            textureCoords,
            normalTexture,
            textureSampler);
    }
    
    // === LIGHT VECTOR CALCULATIONS ===
    // Calculate vector from fragment to light source
    const float3 fragmentToLightVector = lightPositionViewSpace - viewSpacePosition;
    const float distanceToLight = length(fragmentToLightVector);
    const float3 lightDirection = fragmentToLightVector / distanceToLight; // Normalize
    
    // === DISTANCE ATTENUATION ===
    // Light intensity falls off with distance using quadratic formula:
    // Attenuation = 1 / (constant + linear*d + quadratic*d²)
    const float attenuation = 1.0f / (attenuationConstant + 
                                     attenuationLinear * distanceToLight + 
                                     attenuationQuadratic * (distanceToLight * distanceToLight));
    
    // === DIFFUSE LIGHTING (LAMBERTIAN) ===
    // Amount of diffuse light depends on angle between surface normal and light direction
    // Uses dot product: brighter when surface faces light directly
    const float3 diffuseComponent = diffuseLightColor * diffuseLightIntensity * attenuation * 
                                   max(0.0f, dot(lightDirection, surfaceNormal));
    
    // === SPECULAR LIGHTING (BLINN-PHONG) ===
    // Calculate reflection vector for specular highlight
    const float3 lightReflectionVector = normalize(reflect(-lightDirection, surfaceNormal));
    
    // View direction (from fragment toward camera/eye, which is at origin in view space)
    const float3 viewDirection = -normalize(viewSpacePosition);
    
    // Specular intensity based on angle between view direction and reflection
    // Raised to power for tighter highlight control
    const float specularFactor = pow(max(0.0f, dot(lightReflectionVector, viewDirection)), specularShininess);
    const float3 specularComponent = (diffuseLightColor * diffuseLightIntensity) * specularReflectance * 
                                    attenuation * specularFactor;
    
    // === FINAL COLOR COMBINATION ===
    // Sample the diffuse texture
    const float4 textureColor = diffuseTexture.Sample(textureSampler, textureCoords);
    
    // Combine all lighting components:
    // - Ambient + Diffuse are modulated by texture color
    // - Specular is added on top (represents surface reflection, not texture color)
    const float3 finalColor = saturate((ambientLightColor + diffuseComponent) * textureColor.rgb + specularComponent);
    
    return float4(finalColor, 1.0f);
}