cbuffer CBuf
{
    float4 face_colors[6];
};

float4 main(uint triangleId : SV_PrimitiveID) : SV_TARGET
{
	return float4(face_colors[triangleId / 2]);
}