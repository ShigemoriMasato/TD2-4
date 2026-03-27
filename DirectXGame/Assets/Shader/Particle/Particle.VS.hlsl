cbuffer VP : register(b0)
{
    float4x4 gViewProj;
};

StructuredBuffer<float4x4> gWorldMatrices : register(t0);

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
};

VSOutput main(VSInput input, uint instanceID : SV_InstanceID)
{
    VSOutput o;

    float4x4 world = gWorldMatrices[instanceID];

    float4 worldPos = mul(input.position, world);
    o.position = mul(worldPos, gViewProj);

    o.texcoord = input.texcoord;
    o.normal = normalize(mul(input.normal, (float3x3)world));

    return o;
}