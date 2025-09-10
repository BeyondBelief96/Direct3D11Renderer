// =============================================================================
// Blinn-Phong Normal Mapped Vertex Shader
// =============================================================================
// Transforms vertices for normal-mapped models using Blinn-Phong shading.
// Outputs tangent-space basis vectors for normal mapping calculations.
// =============================================================================

// Transform matrices constant buffer
cbuffer TransformMatrices : register(b0)
{
    matrix modelViewMatrix;       // Transform from model space to view (camera) space
    matrix modelViewProjMatrix;   // Transform from model space to clip space for rasterization
};

// Vertex shader output structure
struct VertexOutput
{
    float3 viewSpacePosition : Position;     // Fragment position in view space (for lighting calculations)
    float3 viewSpaceNormal   : Normal;       // Surface normal in view space (for lighting calculations)
    float3 viewSpaceTangent  : Tangent;      // Tangent vector in view space (for normal mapping)
    float3 viewSpaceBitangent: Bitangent;    // Bitangent vector in view space (for normal mapping)
    float2 textureCoords     : TexCoord;     // UV coordinates for texture sampling
    float4 clipSpacePosition : SV_Position;  // Final transformed position for rasterization
};

// Main vertex shader entry point
VertexOutput main(float3 modelPosition  : Position, 
                  float3 modelNormal    : Normal, 
                  float3 modelTangent   : Tangent, 
                  float3 modelBitangent : Bitangent, 
                  float2 texCoords      : TexCoord)
{
    VertexOutput output;
    
    // Transform vertex position from model space to view space
    // This is needed for per-pixel lighting calculations in the pixel shader
    output.viewSpacePosition = mul(float4(modelPosition, 1.0f), modelViewMatrix).xyz;
    
    // Transform the tangent-space basis vectors (TBN matrix components) to view space
    // These vectors define the local coordinate system at each vertex for normal mapping
    // Use only the 3x3 rotation part of the matrix (ignore translation)
    output.viewSpaceNormal    = mul(modelNormal,    (float3x3)modelViewMatrix);
    output.viewSpaceTangent   = mul(modelTangent,   (float3x3)modelViewMatrix);
    output.viewSpaceBitangent = mul(modelBitangent, (float3x3)modelViewMatrix);
    
    // Pass texture coordinates through unchanged
    output.textureCoords = texCoords;
    
    // Transform position to clip space for GPU rasterization
    output.clipSpacePosition = mul(float4(modelPosition, 1.0f), modelViewProjMatrix);
    
    return output;
}