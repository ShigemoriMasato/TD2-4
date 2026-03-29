struct Vertex
{
    float4 position;
    float2 texcoord;
    float3 normal;
};

struct VertexInfluence
{
    float4 weight;
    int4 index;
};

struct SkinningInformation
{
    uint numVertices;
};

struct Well
{
    float4x4 skeletonSpaceMatrix;
    float4x4 skeletonSpaceInverseTransposeMatrix;
};

StructuredBuffer<Well> matrices : register(t0);
StructuredBuffer<Vertex> vertices : register(t1);
StructuredBuffer<VertexInfluence> influences : register(t2);
RWStructuredBuffer<float4x4> output : register(u0);
ConstantBuffer<SkinningInformation> skinninfInfo : register(b0);

float4x4 Multiply(float4x4 a, float b)
{
    return float4x4(
        a[0] * b,
        a[1] * b,
        a[2] * b,
        a[3] * b
    );
}

[numthreads(1024, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint vertexIndex = DTid.x;
    if (vertexIndex >= skinninfInfo.numVertices)
    {
        return;
    }
    
    //必要なデータの抽出
    Vertex input = vertices[vertexIndex];
    VertexInfluence influence = influences[vertexIndex];
    
    float4x4 skinMatrix;
        
    if (influence.weight.x + influence.weight.y + influence.weight.z + influence.weight.w == 0.0)
    {
         skinMatrix = float4x4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
    }
    else
    {
        float4 skinnedPosition = float4(0.0, 0.0, 0.0, 0.0);
        float3 skinnedNormal = float3(0.0, 0.0, 0.0);
        skinMatrix =
            Multiply(matrices[influence.index.x].skeletonSpaceMatrix, influence.weight.x) +
            Multiply(matrices[influence.index.y].skeletonSpaceMatrix, influence.weight.y) +
            Multiply(matrices[influence.index.z].skeletonSpaceMatrix, influence.weight.z) +
            Multiply(matrices[influence.index.w].skeletonSpaceMatrix, influence.weight.w);
    }
        
    output[vertexIndex] = skinMatrix;
}