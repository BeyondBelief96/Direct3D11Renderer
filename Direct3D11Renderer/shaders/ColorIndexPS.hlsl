cbuffer CBuf
{
    float4 face_colors[8];
};

float4 main(uint triangleId : SV_PrimitiveID) : SV_TARGET
{
    return face_colors[(triangleId / 2) % 8];
}