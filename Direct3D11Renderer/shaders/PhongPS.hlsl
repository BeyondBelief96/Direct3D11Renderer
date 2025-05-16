cbuffer Light
{
    float3 lightPos;
};

static const float3 materialColor = { 0.7f, 0.7f, 0.9f };
static const float3 ambientColor = { 0.15f, 0.15f, 0.15f };
static const float3 diffuseColor = { 1.0f, 1.0f, 1.0f };
static const float diffuseIntensity = 1.0f;
static const float attConstant = 1.0f;
static const float attLinear = 0.05f;
static const float attQuadratic = 0.005f;

float4 main(float3 worldPos: Position, float3 normal : Normal) : SV_TARGET
{
    // points from the fragment to the light
    const float3 lightDirection = normalize(lightPos - worldPos);
    const float distanceToLight = length(lightPos - worldPos);
    
    const float attenuation = 1.0f / (attConstant + attLinear * distanceToLight + attQuadratic * (distanceToLight * distanceToLight));
    
    const float diffuse = diffuseColor * diffuseIntensity * attenuation * max(0.0f, dot(lightDirection, normal));
    
    return float4(saturate(diffuse + ambientColor), 1.0f);
}