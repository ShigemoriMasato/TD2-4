#include "PostEffect.hlsli"

cbuffer BlurParams : register(b1)
{
    float intensity; // 最終合成用
    float sigma; // ガウスσ
    int radius; // ブラー半径（例：5～8）
    float pad;
};

cbuffer TextureIndex : register(b0)
{
    int textureIndex; // 使用するテクスチャのインデックス
};

Texture2D<float4> gTexture[] : register(t8);
SamplerState gSampler : register(s0);

float Gaussian1D(float x, float sigma)
{
    return exp(-0.5f * (x * x) / (sigma * sigma));
}

PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput output;

    float2 texelSize = 1.0f / float2(1280.0f, 720.0f);

    float4 color = 0;
    float weightSum = 0;

    for (int y = -radius; y <= radius; y++)
    {
        float w = Gaussian1D(y, sigma);
        float2 offset = float2(0.0f, y * texelSize.y);

        // 横ブラー済みテクスチャを想定（Linearのまま）
        float4 tex = gTexture[textureIndex].Sample(gSampler, input.texcoord + offset);

        color += tex * w;
        weightSum += w;
    }

    color /= weightSum;

    // Linear → sRGB
    color.rgb = pow(color.rgb, 1.0f / 2.2f);

    output.color = color;
    return output;
}
