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

// (i0, i1, i2).(i2, i1, i3) の順で入れなければ殺す
StructuredBuffer<TrailVertex> gVertices : register(t0);

VSOutput main(VSInput input, uint vertexID : SV_VertexID)
{
    VSOutput o;

    // inputなんてねえよ知らねえよ。実際のデータはSRVから読むえ知らないえ。
    TrailVertex ver = gVertices[vertexID];

    o.position = mul(ver.position, vp);
    o.texcoord = ver.uv;
    o.color = ver.color;

    return o;
}