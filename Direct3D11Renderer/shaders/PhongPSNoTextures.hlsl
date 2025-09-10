cbuffer PointLightBuffer : register(b0)
{
    float3 lightPositionViewSpace;
    float3 ambientColor;
    float3 diffuseColor;
    float diffuseIntensity;
    float attenuationConstant;
    float attenuationLinear;
    float attenuationQuadratic;
};

cbuffer ModelBuffer : register(b1)
{
    float4 materialColor;
    float specularIntensity;
    float specularPower;
    float padding[2];
};

float4 main(float3 viewPos : Position, float3 normal : Normal) : SV_TARGET
{
	const float3 fragToLight = lightPositionViewSpace - viewPos;
    const float distanceToLight = length(fragToLight);
    const float3 lightDir = fragToLight / distanceToLight;
    
    // Attenuation
    const float attenuation = 1.0f / (attenuationConstant + attenuationLinear * distanceToLight + attenuationQuadratic * (distanceToLight * distanceToLight));

    // Ambient
    const float3 ambient = attenuation * ambientColor * materialColor.rgb;
    // Diffuse
    const float diff = max(dot(normal, lightDir), 0.0f);
    const float3 diffuse = attenuation * diff * diffuseColor * diffuseIntensity * materialColor.rgb;
    // Specular
    const float3 viewDir = normalize(-viewPos);
    const float3 reflectDir = reflect(-lightDir, normal);
    const float spec = pow(max(dot(viewDir, reflectDir), 0.0f), specularPower);
    const float3 specular = attenuation * spec * diffuseColor * specularIntensity;

    return float4(saturate((diffuse + ambient) + specular), 1.0);

}