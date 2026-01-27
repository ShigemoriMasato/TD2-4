
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

struct Material
{
    float4 color;
};
StructuredBuffer<Material> materialMap : register(t0);

PSOutput main(PSInput input)
{
    PSOutput output;
    Material material = materialMap[input.instanceID];
    output.color = material.color;
    return output;
}
