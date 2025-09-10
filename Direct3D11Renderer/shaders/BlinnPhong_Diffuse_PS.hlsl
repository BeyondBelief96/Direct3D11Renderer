// =============================================================================
// Blinn-Phong Diffuse Pixel Shader
// =============================================================================
// Implements Blinn-Phong lighting model with diffuse texture support.
// Calculates ambient, diffuse, and specular lighting components.
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
    float2 materialPadding;        // Padding for 16-byte alignment requirement
};

// Texture resources
Texture2D diffuseTexture : register(t0);    // Main color/albedo texture
SamplerState textureSampler : register(s0); // Texture sampling settings

// Main pixel shader entry point
float4 main(float3 viewSpacePosition : Position, float3 viewSpaceNormal : Normal, float2 textureCoords : TexCoord) : SV_TARGET
{
    // === RENORMALIZATION ===
    viewSpaceNormal = normalize(viewSpaceNormal);
    
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
                                   max(0.0f, dot(lightDirection, viewSpaceNormal));
    
    // === SPECULAR LIGHTING (BLINN-PHONG) ===
    // Calculate reflection vector for specular highlight
    const float3 lightReflectionVector = reflect(-lightDirection, viewSpaceNormal);
    
    // View direction (from fragment toward camera/eye, which is at origin in view space)
    const float3 viewDirection = normalize(-viewSpacePosition);
    
    // Specular intensity based on angle between view direction and reflection
    // Raised to power for tighter highlight control
    const float specularFactor = pow(max(0.0f, dot(viewDirection, lightReflectionVector)), specularShininess);
    const float3 specularComponent = attenuation * (diffuseLightColor * diffuseLightIntensity) * 
                                    specularReflectance * specularFactor;
    
    // === FINAL COLOR COMBINATION ===
    // Sample the diffuse texture
    const float4 textureColor = diffuseTexture.Sample(textureSampler, textureCoords);
    
    // Combine all lighting components:
    // - Ambient + Diffuse are modulated by texture color
    // - Specular is added on top (represents surface reflection, not texture color)
    const float3 finalColor = saturate((diffuseComponent + ambientLightColor) * textureColor.rgb + specularComponent);
    
    return float4(finalColor, 1.0f);
}