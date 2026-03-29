struct MatrixData
{
    float4x4 world;
};
StructuredBuffer<MatrixData> matrices : register(t0);

cbuffer Camera : register(b0)
{
    float4x4 vp;
};

struct VSInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float4 weight : WEIGHT0;
    int4 boneIndex : INDEX0;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

VSOutput main(VSInput input, uint vertexID : SV_VertexID)
{
    VSOutput output;
    float4x4 world = matrices[vertexID].world;
    output.position = mul(input.position, mul(world, vp));
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float3x3) world));
    return output;
}
