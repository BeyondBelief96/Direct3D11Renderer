cbuffer Light
{
    float3 lightPosViewSpace;   // Light position in view/camera space
    float3 ambientColor;
    float3 diffuseColor;
    float diffuseIntensity;
    float attConstant;
    float attLinear;
    float attQuadratic;
};

cbuffer ObjectConstantBuffer
{
    float3 materialColor;
    float specularIntensity;
    float specularPower;
};

float4 main(float3 posViewSpace: Position, float3 normal : Normal) : SV_TARGET
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
    
    // Ambient light contribution
    float3 ambient = ambientColor * materialColor;
    
    // Calculate diffuse component
    const float diffuseFactor = max(0.0f, dot(normal, lightDirection));
    float3 diffuse = diffuseColor * diffuseFactor * diffuseIntensity * materialColor;
    
    // Calculate specular component using Phong reflection model
    // Reflect expects the first vector to point from light to surface
    float3 reflectionVector = reflect(-lightDirection, normal);
    float specularFactor = pow(max(0.0f, dot(reflectionVector, viewDirection)), specularPower);
    float3 specular = attenuation * diffuseColor * specularFactor * specularIntensity;
    
    // Attenuation
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    
    // Combine all lighting components
    float3 finalColor = (ambient + diffuse + specular);
    
    return float4(saturate(finalColor), 1.0f);
}