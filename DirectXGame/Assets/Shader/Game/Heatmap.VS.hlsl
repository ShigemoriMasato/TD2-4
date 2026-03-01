cbuffer ViewProj : register(b0)
{
    float4x4 viewProj;
};

StructuredBuffer<float4x4> worldMatrices : register(t0);

struct VSInput
{
    float4 position : POSITION;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    uint instanceID : SV_InstanceID;
};

VSOutput main(VSInput input, uint instanceID : SV_InstanceID)
{
    VSOutput output;
    
    float3 pos = input.position.xyz;
    
    float4x4 world = worldMatrices[instanceID];
    float4 worldPos = mul(float4(pos, 1.0f), world);
    output.position = mul(worldPos, viewProj);
    output.instanceID = instanceID;
    
    return output;
}