#include "Block.hlsli"

cbuffer VSData : register(b0)
{
    int2 size;                //ブロックの量
    int2 pad;
    float4x4 parentMatrix;
    float4x4 vpMatrix;
    float4 innerColor;
    float4 outerColor;
};

struct WaveData
{
    float strength;
    float speed;
    float tickness;
    float lifeTime;
    float lifeSpan;
    float2 generatePosition;
    float pad;
    float4 color;
    float3 direction;
    float pad2;
};

StructuredBuffer<WaveData> waveData : register(t0);

struct VSInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

VSOutput main(VSInput input, uint instance : SV_InstanceID)
{
    float x = (instance % size.x) - (size.x / 2);
    float y = (instance / size.x) - (size.y / 2);
    
    float3 local = float3(x, 0.0f, y);
    float4 color = innerColor;
    
    const uint waveCount = 16;
    
    for (uint i = 0; i < waveCount; ++i)
    {
        if (waveData[i].strength <= 0.0f)
        {
            continue;
        }
        
        float rad = waveData[i].lifeTime * waveData[i].speed;
        float dist = distance(local.xz, waveData[i].generatePosition);
        float diff = rad - dist;
        
        if (diff < waveData[i].tickness)
        {
            float lifeStrength = saturate(1.0f - (waveData[i].lifeTime / waveData[i].lifeSpan));
            float fact = saturate((waveData[i].tickness - diff) / waveData[i].tickness) * waveData[i].strength * lifeStrength;
            
            local += waveData[i].direction * fact;
            color += waveData[i].color * fact;
        }
    }
    
    float4 pos = float4(local + input.position.xyz, 1.0f);
    
    VSOutput output;
    output.position = mul(pos, mul(parentMatrix, vpMatrix));
    output.texCoord = input.texcoord;
    output.normal = mul(input.normal, (float3x3) parentMatrix);
    output.color = innerColor;
    output.outlineColor = outerColor;
    return output;
}
