struct PSInput
{
	float4 position : SV_POSITION;
    float3 normal : NORMAL;
};

float4 main(PSInput input) : SV_TARGET
{
	return float4(1.0f, 0.5f, 0.2f, 1.0f); // Warna orange solid
}