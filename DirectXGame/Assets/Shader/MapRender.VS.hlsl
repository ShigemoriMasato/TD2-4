struct VSInput
{
    float4 position : POSITION0;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 worldPos : WORLDPOS0;
    uint instanceID : INSTANCEID0;
};

struct Matrices
{
    float4x4 world;
    float4x4 vp;
};

StructuredBuffer<Matrices> mats : register(t0);

VSOutput main(VSInput input, uint instID : SV_InstanceID)
{
    VSOutput output;
    output.position = mul(input.position, mul(mats[instID].world, mats[instID].vp));
    output.texCoord = input.texCoord;
    output.normal = mul(input.normal, (float3x3) mats[instID].world);
    output.worldPos = mul(input.position, mats[instID].world).xyz;
    output.instanceID = instID;
    return output;
}