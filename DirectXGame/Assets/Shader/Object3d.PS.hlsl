
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
    int textureIndex;
};
ConstantBuffer<Material> gMaterial : register(b0);

Texture2D<float32_t4> gTexture[] : register(t8);
SamplerState gSampler : register(s0);

struct PSOutput
{
    float4 color : SV_TARGET0;
};

PSOutput main(VertexShaderOutput input)
{
    PSOutput output;
    float4 textureColor = gTexture[gMaterial.textureIndex].Sample(gSampler, input.texcoord);
    
    output.color = gMaterial.color * textureColor;
    return output;
}
