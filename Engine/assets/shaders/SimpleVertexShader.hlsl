// File: RenderEngine/EngineCore/assets/shaders/SimpleVertexShader.hlsl
// Deskripsi: Shader vertex sederhana untuk meneruskan posisi dan warna.
// TODO: Tambahkan transformasi MVP di versi berikutnya.

// Struktur input vertex
struct VSInput
{
    float3 position : POSITION;
    float4 color : COLOR;
};

// Struktur output vertex
struct VSOutput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

// Fungsi utama vertex shader
VSOutput main(VSInput input)
{
    VSOutput output;
    output.position = float4(input.position, 1.0); // Tanpa transformasi (placeholder)
    output.color = input.color;
    return output;
}