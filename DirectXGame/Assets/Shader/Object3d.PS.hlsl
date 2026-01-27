
struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD1;
    float3 normal : NORMAL1;
    float3 worldPosition : POSITION1;
};

struct Material
{
    float4 color;
};
ConstantBuffer<Material> gMaterial : register(b0);

//Texture2D<float32_t4> gTexture[] : register(t0);
//SamplerState gSampler : register(s0);

struct PSOutput
{
    float4 color : SV_TARGET0;
};

PSOutput main(VertexShaderOutput input)
{
    PSOutput output;
    output.color = gMaterial.color;
    return output;
}
