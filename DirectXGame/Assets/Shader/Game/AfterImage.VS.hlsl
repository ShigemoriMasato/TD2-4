cbuffer ViewProj : register(b0)
{
    float4x4 viewProj;
};

StructuredBuffer<float4x4> worldMatrices : register(t0);

struct VSInput
{
    float4 position : POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
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
    
    // インスタンスIDに対応するワールド行列を取得
    float4x4 world = worldMatrices[instanceID];
    
    // ワールド座標からプロジェクション座標へ変換
    float4 worldPos = mul(float4(pos, 1.0f), world);
    output.position = mul(worldPos, viewProj);
    
    // ピクセルシェーダーへインスタンスIDを渡す
    output.instanceID = instanceID;
    
    return output;
}