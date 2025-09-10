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
    bool hasGlossChannel;
    bool specularMapEnabled;
    bool normalMapEnabled;
    float specularPowerConstant;
    float specularColor;
    float specularMapWeight;
};

Texture2D diffuseMap : register(t0);
Texture2D specularMap : register(t1);
Texture2D normalMap : register(t2);

SamplerState samplerState;

float4 main(float3 fragPosCamera : Position, float3 normal : Normal, float3 tangent : Tangent, float3 bitangent : Bitangent, float2 texCoord : TexCoord) : SV_Target
{
    if (normalMapEnabled)
    {
        // Build the TBN matrix
        const float3x3 tangentToViewSpace = float3x3(normalize(tangent), normalize(bitangent), normalize(normal));
        // Sample normal from normal map and transform to range [-1, 1]
        // Note: Invert Y component to account for different texture coordinate system
        const float3 normalSample = normalMap.Sample(samplerState, texCoord).xyz;
        normal = normalSample * 2.0f - 1.0f;
        normal.y = -normal.y; // Invert Y component

        
        // transform normal by modelView matrix (upper 3x3)
        normal = mul(normal, tangentToViewSpace);
    }

    const float3 fragToLight = lightPos - fragPosCamera;
    const float3 distanceToLight = length(fragToLight);
    const float3 directionToLight = fragToLight / distanceToLight;
    
    const float attenuation = 1.0f / (attenuationConstant + attenuationLinear * distanceToLight + attenuationQuadratic * (distanceToLight * distanceToLight));
    
    const float3 diffuse = diffuseColor * diffuseIntensity * attenuation * max(0.0f, dot(directionToLight, normal));
    
    const float3 reflectDir = normalize(reflect(-directionToLight, normal));
    const float4 specularSample = specularMap.Sample(samplerState, texCoord);
    const float3 specularReflectionColor;

    float specularPower = specularPowerConstant;
    if(specularMapEnabled)
    {
        const float4 specularSample = specularMap.Sample(samplerState, texCoord);
        specularReflectionColor = specularSample.rgb * specularMapWeight;
        if (hasGlossChannel)
        {
            specularPower = pow(2.0f, specularSample.a * 13.0f);
        }
    }
   
    else
    {
        specularPower = specularPowerConstant;
    }
    
    const float3 specular = (diffuseColor * diffuseIntensity) * attenuation * pow(max(0.0f, dot(reflectDir, -normalize(fragPosCamera))), specularPower);
    
    return float4(saturate((ambientColor + diffuse) * diffuseMap.Sample(samplerState, texCoord).rgb + specular * specularReflectionColor), 1.0f);
};