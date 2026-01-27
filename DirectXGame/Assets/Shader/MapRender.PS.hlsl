
struct PSInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    uint instanceID : INSTANCEID0;
};

struct PSOutput
{
    float4 color : SV_TARGET;
};

Texture2D<float4> gTexture[] : register(t8);
SamplerState gSampler : register(s0);

struct Color
{
    float4 color;
};
StructuredBuffer<Color> colors : register(t0);
struct TextureIndex
{
    int index;
};
StructuredBuffer<TextureIndex> textureIndices : register(t1);

PSOutput main(PSInput input)
{
    PSOutput output;
    float4 color = colors[input.instanceID].color;
    int textureIndex = textureIndices[input.instanceID].index;
    output.color = gTexture[textureIndex].Sample(gSampler, input.texcoord) * color;
    return output;
}
