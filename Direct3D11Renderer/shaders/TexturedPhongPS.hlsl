cbuffer Light
{
    float3 lightPosViewSpace; // Light position in view/camera space
    float3 ambientColor;
    float3 diffuseColor;
    float diffuseIntensity;
    float attConstant;
    float attLinear;
    float attQuadratic;
};

cbuffer ObjectConstantBuffer
{
    float specularIntensity;
    float specularPower;
};

Texture2D texDiffuse;
SamplerState splr;

float4 main(float3 posViewSpace : Position, float3 normal : Normal, float2 texCoord : TexCoord) : SV_TARGET
{
    // Sample the texture for the base diffuse color
    float4 texColor = texDiffuse.Sample(splr, texCoord);
    
    // Normalize the interpolated normal
    normal = normalize(normal);
    
    // View direction is negative of fragment position in view space
    const float3 viewDirection = normalize(-posViewSpace);
    
    // Calculate direction from fragment to light in view space
    const float3 lightDirection = normalize(lightPosViewSpace - posViewSpace);
    
    // Calculate distance to light and attenuation
    const float distanceToLight = length(lightPosViewSpace - posViewSpace);
    const float attenuation = 1.0f / (attConstant + attLinear * distanceToLight + attQuadratic * (distanceToLight * distanceToLight));
    
    // Ambient light contribution (using texture color)
    float3 ambient = ambientColor * texColor.rgb;
    
    // Calculate diffuse component (using texture color)
    const float diffuseFactor = max(0.0f, dot(normal, lightDirection));
    float3 diffuse = diffuseColor * diffuseFactor * diffuseIntensity * texColor.rgb;
    
    // Calculate specular component using Phong reflection model
    float3 reflectionVector = reflect(-lightDirection, normal);
    float specularFactor = pow(max(0.0f, dot(reflectionVector, viewDirection)), specularPower);
    float3 specular = diffuseColor * specularFactor * specularIntensity;
    
    // Apply attenuation to all components
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    
    // Combine all lighting components
    float3 finalColor = ambient + diffuse + specular;
    
    return float4(saturate(finalColor), texColor.a);
}