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
    float4 color : COLOR0;
};

cbuffer VPBuffer : register(b0)
{
    float4x4 vp;
};

struct TrailVertex
{
    float4 position;
    float2 uv;
    float3 normal;
    float4 color;
};

StructuredBuffer<TrailVertex> gVertices : register(t0);

VSOutput main(VSInput input, uint vertexID : SV_VertexID)
{
    VSOutput o;

    // 入力VBはダミー。実データはSRVから読む
    TrailVertex ver = gVertices[vertexID];

    o.position = mul(ver.position, vp);
    o.texcoord = ver.uv;
    o.color = ver.color;

    return o;
}