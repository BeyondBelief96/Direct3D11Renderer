cbuffer LightCBuf
{
    float3 lightPos;
    float3 ambientColor;
    float3 diffuseColor;
    float diffuseIntensity;
    float attenuationConstant;
    float attenuationLinear;
    float attenuationQuadratic;
};

cbuffer ObjectCBuf
{
    float specularIntensity;
    float specularPower;
    bool normalMapEnabled;
    float padding[1];
};

Texture2D diffuseMap;
Texture2D normalMap;
SamplerState samplerState;

float4 main(float3 fragPosCamera : Position, float3 normal : Normal, float2 texCoord : TexCoord) : SV_Target
{
    if (normalMapEnabled)
    {
        const float3 normalSample = normalMap.Sample(samplerState, texCoord).xyz;
        normal.x = normalSample.x * 2.0f - 1.0f;
        normal.y = -normalSample.y * 2.0f + 1.0f;
        normal.z = -normalSample.z;
    }
    
    const float3 fragToLight = lightPos - fragPosCamera;
    const float3 distanceToLight = length(fragToLight);
    const float3 directionToLight = fragToLight / distanceToLight;
    
    const float attenuation = 1.0f / (attenuationConstant + attenuationLinear * distanceToLight + attenuationQuadratic * (distanceToLight * distanceToLight));
    
    const float3 diffuse = diffuseColor * diffuseIntensity * attenuation * max(0.0f, dot(directionToLight, normal));
    
    const float3 reflectDir = normalize(reflect(-directionToLight, normal));
    const float3 specular = (diffuseColor * diffuseIntensity) * specularIntensity * attenuation * pow(max(0.0f, dot(reflectDir, -normalize(fragPosCamera))), specularPower);
    
    return float4(saturate((ambientColor + diffuse) * diffuseMap.Sample(samplerState, texCoord).rgb + specular), 1.0f);
};