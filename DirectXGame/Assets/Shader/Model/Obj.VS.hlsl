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
};

struct VSData
{
    float4x4 world;
    float4x4 wvp;
};
StructuredBuffer<VSData> matrices : register(t0);

VSOutput main(VSInput input, uint id : SV_InstanceID)
{
    VSOutput output;
    output.position = mul(input.position, matrices[id].wvp);
    output.texCoord = input.texCoord;
    output.normal = mul(input.normal, (float3x3) matrices[id].world);
    output.worldPos = mul(input.position, matrices[id].world).xyz;
    return output;
}