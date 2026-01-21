
struct VSOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD1;
};

cbuffer MatrixBuffer : register(b0)
{
    float4x4 wvp;
}

struct VSInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

struct StringUV
{
    float2 uv0;
    float2 uv1;
    float advanceX;
    float bearingX;
    float bearingY;
    float penX;
};
StructuredBuffer<StringUV> gUVs : register(t0);

VSOutput main(VSInput input, uint instanceId : SV_InstanceID, uint vertexID : SV_VertexID)
{
    VSOutput output;

    // ===== 座標変換 =====
    output.position = mul(input.position, wvp);
    output.position.x += instanceId * wvp[3][0];

    // ===== UV変換 =====
    output.texcoord = 
    float2(
    (vertexID == 0 || vertexID == 1 ? gUVs[instanceId].uv0.x : gUVs[instanceId].uv1.x),
    (vertexID == 0 || vertexID == 3 ? gUVs[instanceId].uv0.y : gUVs[instanceId].uv1.y)
    );

    return output;
}