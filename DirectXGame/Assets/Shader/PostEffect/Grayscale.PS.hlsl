#include "PostEffect.hlsli"

cbuffer GrayscaleParameters : register(b1)
{
    float intensity; // グレースケールの強度（0.0〜1.0）
}

cbuffer TextureIndex : register(b0)
{
    int textureIndex; // 使用するテクスチャのインデックス
};

Texture2D<float4> gTexture[] : register(t8);
SamplerState gSampler : register(s0);

PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput output;

    float clampedIntensity = clamp(intensity, 0.0, 1.0);
    
    // 元の色をサンプリング
    float4 color = gTexture[textureIndex].Sample(gSampler, input.texcoord);

    // グレースケール変換（人間の視覚に基づく加重平均）
    float luminance = dot(color.rgb, float3(0.299, 0.587, 0.114));

    // グレースケール色を作成
    float3 grayscale = float3(luminance, luminance, luminance);

    // 強度で補間（カラーとグレースケールのブレンド）
    float3 finalColor = lerp(color.rgb, grayscale, clampedIntensity);

    // 出力
    output.color = float4(finalColor, 1.0);
    return output;
}
