struct PSInput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

struct PSOutput
{
    float4 color : SV_TARGET;
};

cbuffer Color : register(b0)
{
    float4 color;
};

cbuffer TextureIndex : register(b1)
{
    int textureIndex;
};

Texture2D textures[] : register(t8);
SamplerState gSampler : register(s0);

PSOutput main(PSInput input)
{
    PSOutput output;
    
    // テクスチャをサンプリング
    float4 texColor = textures[textureIndex].Sample(gSampler, input.texCoord);
    
    // テクスチャとカラーの乗算
    output.color = texColor * color;
    
    return output;
}