RWStructuredBuffer<uint> freeList : register(u0);
RWStructuredBuffer<uint> freeListIndex : register(u1);
RWStructuredBuffer<float4x4> wvpMatricies : register(u2);
RWStructuredBuffer<float> lifeTimes : register(u3);

cbuffer ParticleNum : register(b0)
{
    uint maxNum;
};
cbuffer DeadTime : register(b1)
{
    float deadTime;
};

[numthreads(256, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    if (DTid.x >= maxNum)
    {
        return;
    }
    uint index = DTid.x;
    
    //FreeListの初期化
    freeList[index] = index;
    if (DTid.x == 0)//同時に書き込まないようにするため
    {
        freeListIndex[0] = maxNum - 1;
    }
    
    //パラメータの初期化
    wvpMatricies[index] = float4x4(0, 0, 0, 0,
                                   0, 0, 0, 0,
                                   0, 0, 0, 0,
                                   0, 0, 0, 0);
    lifeTimes[index] = deadTime + 1;
}