cbuffer Light
{
    float3 lightPosViewSpace;
    float3 ambientColor;
    float3 diffuseColor;
    float diffuseIntensity;
    float attConstant;
    float attLinear;
    float attQuadratic;
};

Texture2D diff;
Texture2D specular;
SamplerState splr;

float4 main(float3 posViewSpace : Position, float3 normal : Normal, float2 texCoord : TexCoord) : SV_TARGET
{
    // In camera space, the normal may still need normalization due to interpolation
    normal = normalize(normal);
    
    // In camera space, the camera is at (0,0,0), so the view direction is just
    // the negative of the fragment position
    const float3 viewDirection = normalize(-posViewSpace);
    
    // Calculate direction from fragment to light in view space
    const float3 lightDirection = normalize(lightPosViewSpace - posViewSpace);
    
    // Calculate distance to light
    const float distanceToLight = length(lightPosViewSpace - posViewSpace);
    const float attenuation = 1.0f / (attConstant + attLinear * distanceToLight + attQuadratic * (distanceToLight * distanceToLight));
   
    // Sample diffuse texture
    const float4 diffuseSample = diff.Sample(splr, texCoord);
    
    // Calculate diffuse component
    const float diffuseFactor = max(0.0f, dot(lightDirection, normal));
    float3 diffuse = diffuseColor * diffuseFactor * diffuseIntensity * attenuation;
    
    // Calculate specular component using Phong reflection model
    float3 reflectionVector = normalize(reflect(-lightDirection, normal));
    
    const float4 specularSample = specular.Sample(splr, texCoord);
    const float3 specularColorIntensity = specularSample.rgb;
    const float specularPower = specularSample.a;
    float specularFactor = pow(max(0.0f, dot(reflectionVector, viewDirection)), specularPower);
    float3 specularColor = specularFactor * specularColorIntensity * attenuation;
    
    // Calculate ambient with attenuation
    float3 ambient = ambientColor * attenuation;
    
    // Combine all lighting components and modulate with diffuse texture
    float3 finalColor = ambient + diffuse + specularColor;
    
    return float4(saturate(finalColor), 1.0f) * diff.Sample(splr, texCoord);
}