cbuffer VP : register(b0)
{
    float4x4 gViewProj;
};

struct ParticleInstanceData
{
    float4x4 world;
    uint textureIndex;
    float4 color;
};
StructuredBuffer<ParticleInstanceData> gInstances : register(t0);

struct VSInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    nointerpolation uint textureIndex : TEXCOORD1;
    float4 color : COLOR0;
};

VSOutput main(VSInput input, uint instanceID : SV_InstanceID)
{
    VSOutput o;

    ParticleInstanceData inst = gInstances[instanceID];
    float4x4 world = inst.world;

    float4 worldPos = mul(input.position, world);
    o.position = mul(worldPos, gViewProj);

    o.texcoord = input.texcoord;
    o.normal = normalize(mul(input.normal, (float3x3)world));

    o.textureIndex = inst.textureIndex;
    o.color = inst.color;

    return o;
}