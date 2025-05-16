cbuffer Light
{
    float3 lightPos;
    float3 materialColor;
    float3 ambientColor;
    float3 diffuseColor;
    float diffuseIntensity;
    float attConstant;
    float attLinear;
    float attQuadratic;
};

float4 main(float3 worldPos: Position, float3 normal : Normal) : SV_TARGET
{
    // points from the fragment to the light
    const float3 lightDirection = normalize(lightPos - worldPos);
    const float distanceToLight = length(lightPos - worldPos);
    
    const float attenuation = 1.0f / (attConstant + attLinear * distanceToLight + attQuadratic * (distanceToLight * distanceToLight));
    
    const float diffuse = diffuseColor * diffuseIntensity * attenuation * max(0.0f, dot(lightDirection, normal));
    
    return float4(saturate((diffuse + ambientColor) * materialColor), 1.0f);
}