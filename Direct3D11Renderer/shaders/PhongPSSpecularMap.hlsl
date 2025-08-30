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

cbuffer ObjectConstantBuffer
{
    float specularIntensity;
    float specularPower;
    float padding[2]; // Padding to ensure 16-byte alignment
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
   
    float ambient = ambientColor;
    
    // Calculate diffuse component
    float3 diffuse = diffuseColor * diffuseIntensity * max(0.0f, dot(lightDirection, normal));
    
    // Calculate specular component using Phong reflection model
    // Reflect expects the first vector to point from light to surface
    float3 reflectionVector = normalize(reflect(-lightDirection, normal));
    float4 specularSample = specular.Sample(splr, texCoord);
    const float3 specularColorIntensity = specularSample.rgb;
    const float specularPowerFromMap = specularSample.a; // Renamed to avoid conflict
    float3 specular = specularColorIntensity * pow(max(0.0f, dot(reflectionVector, viewDirection)), specularPowerFromMap) * specularIntensity;
    
    // Attenuation
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    
    // Combine all lighting components
    float3 finalColor = (ambient + diffuse + specular);
    
    // Sample texture and modulate with lighting result
    return float4(saturate(finalColor), 1.0f) * diff.Sample(splr, texCoord);
}