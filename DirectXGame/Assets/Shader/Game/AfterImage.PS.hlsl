
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

struct ColorBuffer
{
    float4 color;
};
StructuredBuffer<ColorBuffer> colors : register(t0);
StructuredBuffer<int> textureIndices : register(t1);
Texture2D<float4> textures[] : register(t8);
SamplerState gSampler : register(s0);

PSOutput main(PSInput input)
{
    PSOutput output;

    float4 color = colors[input.instanceID].color;
    int texIdx = textureIndices[input.instanceID];
    float4 texColor = textures[texIdx].Sample(gSampler, input.texCoord);
    output.color = color * texColor;
    return output;
}
