cbuffer Matrices : register(b0)
{
    float4x4 worldViewProj;
}

struct VSInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
};

PSInput main(VSInput input)
{
    PSInput output;
    output.position = mul(float4(input.position, 1.0f), worldViewProj);
    output.normal = input.normal;
    return output;
}