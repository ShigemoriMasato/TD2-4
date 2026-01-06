
struct VSOutput
{
    float4 posH : SV_POSITION;
    float3 worldPos : TEXCOORD0;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD1;
    float3 viewDir : TEXCOORD2;
};

struct PSOutput
{
    float4 color : SV_TARGET;
};

struct DirectionalLight
{
    float4 color;
    float3 direction;
    float intensity;
};

cbuffer Data : register(b0)
{
    DirectionalLight dirLight;
}

cbuffer Color : register(b1)
{
    float4 baseColor;
}

float4 HalfLambert(float3 normal, float4 color, DirectionalLight light)
{
    float NdotL = dot(normalize(normal), -light.direction);
    float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
    float4 outputColor = color * light.color * cos * light.intensity;
    outputColor.w = color.w;
    return outputColor;
}

float4 LambertReflectance(float3 normal, float4 color, DirectionalLight light)
{
    float NdotL = dot(normalize(normal), -light.direction);
    float cos = saturate(dot(normalize(normal), -light.direction));
    float4 outputColor = color * light.color * cos * light.intensity;
    outputColor.w = color.w;
    return outputColor;
}

PSOutput main(VSOutput input)
{
    //いったん
    PSOutput output;
    output.color = baseColor;
    output.color += HalfLambert(input.normal, output.color, dirLight);
    return output;
}
