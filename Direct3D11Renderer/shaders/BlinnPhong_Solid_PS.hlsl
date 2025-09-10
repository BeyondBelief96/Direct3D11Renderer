// =============================================================================
// Blinn-Phong Solid Color Pixel Shader
// =============================================================================
// Implements Blinn-Phong lighting model with solid material color (no textures).
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
    float4 materialDiffuseColor;   // Base color of the material (RGB + Alpha)
    float  specularReflectance;    // How much specular light the material reflects (0-1)
    float  specularShininess;      // Controls specular highlight size (higher = smaller highlight)
    float2 materialPadding;        // Padding for 16-byte alignment requirement
};

// Main pixel shader entry point
float4 main(float3 viewSpacePosition : Position, float3 viewSpaceNormal : Normal) : SV_TARGET
{
    // === RENORMALIZE SURFACE NORMAL ===
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
    
    // === AMBIENT LIGHTING ===
    // Base lighting that's always present (simulates environmental light)
    const float3 ambientComponent = attenuation * ambientLightColor * materialDiffuseColor.rgb;
    
    // === DIFFUSE LIGHTING (LAMBERTIAN) ===
    // Amount of diffuse light depends on angle between surface normal and light direction
    // Uses dot product: brighter when surface faces light directly
    const float diffuseDotProduct = max(dot(viewSpaceNormal, lightDirection), 0.0f);
    const float3 diffuseComponent = attenuation * diffuseDotProduct * diffuseLightColor * 
                                   diffuseLightIntensity * materialDiffuseColor.rgb;
    
    // === SPECULAR LIGHTING (BLINN-PHONG) ===
    // View direction (from fragment toward camera/eye, which is at origin in view space)
    const float3 viewDirection = normalize(-viewSpacePosition);
    
    // Calculate reflection of light direction about the surface normal
    const float3 lightReflectionVector = reflect(-lightDirection, viewSpaceNormal);
    
    // Specular intensity based on angle between view direction and reflection
    // Raised to power for tighter highlight control
    const float specularFactor = pow(max(dot(viewDirection, lightReflectionVector), 0.0f), specularShininess);
    const float3 specularComponent = attenuation * specularFactor * diffuseLightColor * specularReflectance;
    
    // === FINAL COLOR COMBINATION ===
    // Combine all lighting components and saturate to prevent over-bright colors
    const float3 finalColor = saturate(diffuseComponent + ambientComponent + specularComponent);
    
    return float4(finalColor, 1.0f);
}