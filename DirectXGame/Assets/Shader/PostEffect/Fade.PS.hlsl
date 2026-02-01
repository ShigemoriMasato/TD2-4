#include "PostEffect.hlsli"

cbuffer FadeParameters : register(b1)
{
    float3 FadeColor; // フェードカラー
    float FadeAmount; // フェードの進行度 (0.0 = no fade, 1.0 = full fade)
}

cbuffer TextureIndex : register(b0)
{
    int textureIndex; // 使用するテクスチャのインデックス
};

Texture2D<float4> gTexture[] : register(t8);
SamplerState gSampler : register(s0);

PixelShaderOutput main(PixelShaderInput input) {
    PixelShaderOutput output;
    // テクスチャから色を取得
    float4 color = gTexture[textureIndex].Sample(gSampler, input.texcoord);
    float alpha = color.a;
    
    output.color = lerp(color, float4(FadeColor, alpha), FadeAmount);
    
    return output;
}
