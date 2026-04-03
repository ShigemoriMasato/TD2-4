
struct PSInput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL0;
    uint instanceID : INSTANCE0;
};

struct PSOutput
{
    float4 color : SV_TARGET;
};

cbuffer TextureIndex : register(b0)
{
    int textureIndex;
};

cbuffer ColorBuffer : register(b1)
{
    float4 color;
};

Texture2D<float4> textures[] : register(t8);
SamplerState gSampler : register(s0);

PSOutput main(PSInput input)
{
    PSOutput output;
    float4 texColor = textures[textureIndex].Sample(gSampler, input.texCoord);
    output.color = color * texColor;
    return output;
}
