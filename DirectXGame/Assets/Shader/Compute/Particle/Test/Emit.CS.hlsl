cbuffer InitData : register(b0)
{
    float3 position;
    float lifeTime;
    float3 velocity;
};
cbuffer ParticleNum : register(b1)
{
    uint maxNum;
};
cbuffer EmitConfig : register(b2)
{
    int emitCount;
    int isEmit;
};

RWStructuredBuffer<uint> freeList : register(u0);
RWStructuredBuffer<uint> freeListIndex : register(u1);
RWStructuredBuffer<float3> positions : register(u2);
RWStructuredBuffer<float3> velocities : register(u3);
RWStructuredBuffer<float> lifetimes : register(u4);

[numthreads(256, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    if (isEmit == 0)
    {
        return;
    }
    
    uint index = DTid.x;
    if (index >= emitCount)
    {
        return;
    }
    
    if (freeListIndex[0] <= 0)
    {
        return;
    }
    int freeIndex;
    InterlockedAdd(freeListIndex[0], -1, freeIndex);
    
    uint particleIndex = freeList[freeIndex];
    positions[particleIndex] = position;
    velocities[particleIndex] = velocity;
    lifetimes[particleIndex] = lifeTime;
}