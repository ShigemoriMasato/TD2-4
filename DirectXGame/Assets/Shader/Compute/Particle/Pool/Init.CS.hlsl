RWStructuredBuffer<uint> freeList : register(u0);
RWStructuredBuffer<uint> freeListIndex : register(u1);
RWStructuredBuffer<uint> type : register(u2);

cbuffer ParticleNum : register(b0)
{
    uint maxNum;
};

[numthreads(128, 1, 1)]

void main(uint3 DTid : SV_DispatchThreadID )
{
    if (DTid.x >= maxNum)
    {
        return;
    }
    uint index = DTid.x;
    
    freeList[index] = maxNum - 1 - index; //FreeListの初期化
    if (DTid.x == 0)//同時に書き込まないようにするため
    {
        freeListIndex[0] = maxNum - 1;
    }

    type[index] = 0; //タイプの初期化
}