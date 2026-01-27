
struct PSInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

struct PSOutput
{
    float4 color : SV_TARGET;
};

Texture2D<float4> gTexture[] : register(t8);
SamplerState gSampler : register(s0);

cbuffer TextureIndexBuffer : register(b0)
{
    uint textureIndex;
}

cbuffer ColorBuffer : register(b1)
{
    float4 color;
}

PSOutput main(PSInput input)
{
    PSOutput output;
    output.color = gTexture[textureIndex].Sample(gSampler, input.texcoord) * color;
    return output;
}
