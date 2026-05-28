#include "PostEffect.hlsli"

cbuffer TextureIndex : register(b0)
{
    int textureIndex;
};

Texture2D<float4> textures[] : register(t8);
SamplerState gSampler : register(s0);

cbuffer Data : register(b1)
{
    float4 color; // 色
    float intensity; // Bloo の強さ
    float radius; // Bloomの広がる半径
    float softness; // グラデーションの滑らかさ
};

float4 main(PixelShaderInput input) : SV_TARGET
{
    float2 uv = input.texcoord;

    // 中心からの距離
    float2 center = float2(0.5, 0.5);
    float dist = distance(uv, center);

    // Bloom係数
    float bloomFactor = smoothstep(radius, radius - softness, dist);

    // 強さ適用
    bloomFactor *= intensity;

    // 元の色
    float4 baseColor = textures[textureIndex].Sample(gSampler, uv);

    // Bloom 色を加算
    float3 bloomColor = color.rgb * bloomFactor;

    float3 finalColor = baseColor.rgb + bloomColor;

    return float4(finalColor, 1.0f);
}
