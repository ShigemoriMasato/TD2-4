cbuffer ParticleNum : register(b0)
{
    uint maxNum;
};
cbuffer CSData : register(b1)
{
    float deltaTime;
    float3 color;
    float lifetime;
    float3 fieldSize;
    float4x4 parentMatrix;
};

struct Wave
{
    float3 position;
    float speed;
    
    float3 color;
    float intensity;
    
    float lifetime;
    float decayRate;
    float maxlifetime;
    float thickness;
};
cbuffer WaveBuffer : register(b2)
{
    Wave waves[16];
}

RWStructuredBuffer<uint> freeList : register(u0);
RWStructuredBuffer<uint> freeListIndex : register(u1);
RWStructuredBuffer<float3> outPositions : register(u2);
RWStructuredBuffer<float4> colors : register(u3);
RWStructuredBuffer<float3> velocities : register(u4);
RWStructuredBuffer<float> lifetimes : register(u5);
RWStructuredBuffer<float3> positions : register(u6);
RWStructuredBuffer<uint> isUse : register(u7);

[numthreads(256, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint index = DTid.x;
    if (index >= maxNum)
    {
        return;
    }
    if (isUse[index] == 0)
    {
        colors[index] = float4(0, 0, 0, 0);
        return;
    }
    
    positions[index] += velocities[index] * deltaTime;
    
    //波の処理
    float3 pos = positions[index];
    float3 col = color;
    const int kWaveNum = 16;
    for (int i = 0; i < 16; ++i)
    {
        if (waves[i].lifetime >= waves[i].maxlifetime || waves[i].maxlifetime == 0)
        {
            continue;
        }
        
        float dist = length(pos.xz - waves[i].position.xz);
        float radius = waves[i].speed * waves[i].lifetime;
        float diff = dist - radius;
        float range = waves[i].thickness;
        
        if (diff > -range && diff < range)
        {
            float decay = 1.0f - (waves[i].lifetime / waves[i].maxlifetime);
            float intensity = (1.0f - abs(diff) / range) * waves[i].intensity * decay;
            pos += float3(0, intensity, 0);
            col += waves[i].color * intensity;
        }
    }
    
    //出力先に値を書き込む
    float alpha = abs(float(lifetimes[index] / (lifetime * 0.5f)) - 1);
    colors[index] = float4(col, alpha);
    
    outPositions[index] = mul(float4(pos, 1), parentMatrix).xyz;
    
    lifetimes[index] -= deltaTime;
    if (lifetimes[index] <= 0.0f)
    {
        uint freeIndex;
        InterlockedAdd(freeListIndex[0], 1, freeIndex);
        freeList[freeIndex + 1] = index;
        isUse[index] = 0;
        colors[index] = float4(0, 0, 0, 0);
    }
}
