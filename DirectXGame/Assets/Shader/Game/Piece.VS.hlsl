
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

struct MatrixStruct
{
    float4x4 world;
};

StructuredBuffer<MatrixStruct> matrices : register(t0);

cbuffer Camera : register(b0)
{
    matrix vp;
};

VSOutput main(VSInput input, uint id : SV_InstanceID)
{
    VSOutput output;

    float4x4 world = matrices[id].world;
    float4 worldPos = mul(input.position, world);

    output.position = mul(worldPos, vp);
    output.texcoord = input.texcoord;
    output.normal = mul(input.normal, (float3x3)world);

    return output;
}
