Texture2D tex;
SamplerState samplerState;

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    return tex.Sample(samplerState, texCoord);
}