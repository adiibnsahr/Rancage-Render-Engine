// File: RenderEngine/EngineCore/assets/shaders/SimplePixelShader.hlsl
// Deskripsi: Shader pixel sederhana untuk mengembalikan warna vertex.

// Struktur input pixel
struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

// Fungsi utama pixel shader
float4 main(PSInput input) : SV_TARGET
{
    return input.color; // Mengembalikan warna langsung
}