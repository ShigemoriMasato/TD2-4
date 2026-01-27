
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

struct Material
{
    float4 color;
    uint textureIndex;
    uint3 padding;
};
StructuredBuffer<Material> materialMap : register(t0);

PSOutput main(PSInput input)
{
    PSOutput output;
    Material material = materialMap[input.instanceID];
    output.color = gTexture[material.textureIndex].Sample(gSampler, input.texcoord) * material.color;
    return output;
}
