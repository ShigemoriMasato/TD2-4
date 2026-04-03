struct VSInput
{
    float3 position : POSITION0;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

cbuffer MatrixBuffer : register(b0)
{
    float4x4 wvp;
}

StructuredBuffer<float2> positions : register(t0);

VSOutput main(VSInput input, uint instance : SV_InstanceID, uint vertex : SV_VertexID)
{
    VSOutput output;
    uint currentIndex = instance + vertex;
    float4 pos = float4(positions[currentIndex], 0.0f, 1.0f);
    
    
    output.position = mul(pos, wvp);
    output.texCoord = float2(0.0f, 0.0f);
    output.normal = float3(0.0f, 0.0f, -1.0f);
    
    return output;
}