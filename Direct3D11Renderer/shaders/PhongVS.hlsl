cbuffer CBuf
{
    matrix modelView; // Transforms vertices from model space to camera space
    matrix modelViewProj; // Transforms vertices from model space to clip space
};

struct VSOut
{
    float3 posViewSpace : Position; // Position in camera/view space
    float3 normal : Normal; // Normal in camera/view space
    float4 pos : SV_Position; // Position in clip space
};

VSOut main(float3 pos : Position, float3 normal : Normal)
{
    VSOut vso;
    // Transform position to camera space
    vso.posViewSpace = (float3) mul(float4(pos, 1.0f), modelView);
    // Transform normal to camera space 
    vso.normal = normalize(mul(normal, (float3x3) modelView));
    // Transform position to clip space
    vso.pos = mul(float4(pos, 1.0f), modelViewProj);
    return vso;
}