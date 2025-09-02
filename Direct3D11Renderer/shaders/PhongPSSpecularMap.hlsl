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
    // fragment to light vector data
    const float3 vToL = lightPosViewSpace - posViewSpace;
    const float distToL = length(vToL);
    const float3 dirToL = vToL / distToL;
	// attenuation
    const float att = 1.0f / (attConstant + attLinear * distToL + attQuadratic * (distToL * distToL));
	// diffuse intensity
    const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(dirToL, normal));
	// reflected light vector
    const float3 r = reflect(-dirToL, normal);
	// calculate specular intensity based on angle between viewing vector and reflection vector, narrow with power function
    const float4 specularSample = specular.Sample(splr, texCoord);
    const float3 specularColorIntensity = specularSample.rgb;
    const float specularPower = pow(2.0f, specularSample.a * 13.0f);
    const float3 specular = att * specularColorIntensity * pow(max(0.0f, dot(normalize(-r), normalize(posViewSpace))), specularPower);
	// final color
    return float4(saturate((diffuse + ambientColor) * diff.Sample(splr, texCoord).rgb + specular * specularColorIntensity), 1.0f);
}