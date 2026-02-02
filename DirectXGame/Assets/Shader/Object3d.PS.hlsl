
struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD1;
    float3 normal : NORMAL1;
    float3 worldPosition : POSITION1;
};

struct Material
{
    float4 color;
    int textureIndex;
    float shininess;
    int isActive;
};
ConstantBuffer<Material> gMaterial : register(b0);

struct DirectionalLight
{
    float4 color;
    float3 direction;
    float intensity;
};
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

cbuffer CameraData : register(b2)
{
    float3 cameraWorldPos;
    float padding;
}

Texture2D<float4> gTexture[] : register(t8);
SamplerState gSampler : register(s0);

struct PSOutput
{
    float4 color : SV_TARGET0;
};

float3 CalculateShading(float3 lightDirection, float3 lightColor, float3 normal, float3 viewDirection, float3 materialColor, Material matData);
float3 CalculateDirectionalLight(DirectionalLight light, float3 normal, float3 viewDirection, float3 materialColor, Material matData);

PSOutput main(VertexShaderOutput input)
{
    PSOutput output;
    float4 textureColor = gTexture[gMaterial.textureIndex].Sample(gSampler, input.texcoord);
    
    if (gMaterial.isActive)
    {
        // 最終的な色
        float3 finalColor = float3(0.0f, 0.0f, 0.0f);
        
        // ライト計算のための共通データを準備
        float3 normal = normalize(input.normal);
        float3 toEye = normalize(cameraWorldPos - input.worldPosition);
        float3 baseColor = gMaterial.color.rgb * textureColor.rgb;
        
        finalColor += CalculateDirectionalLight(gDirectionalLight, normal, toEye, baseColor, gMaterial);
        
         // 最終的な色を設定
        output.color.rgb = finalColor;
        output.color.a = gMaterial.color.a * textureColor.a;
    }
    else
    {
        output.color = gMaterial.color * textureColor;
    }
    return output;
}

float3 CalculateShading(float3 lightDirection, float3 lightColor, float3 normal, float3 viewDirection, float3 materialColor, Material matData)
{
    float NdotL = dot(normal, lightDirection);
    float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
    float3 diffuse = materialColor * lightColor * cos;

    float3 halfVector = normalize(lightDirection + viewDirection);
    float NDotH = dot(normal, halfVector);
    float specularPow = pow(saturate(NDotH), matData.shininess);
    float3 specular = lightColor * specularPow;

    return diffuse + specular;
}

float3 CalculateDirectionalLight(DirectionalLight light, float3 normal, float3 viewDirection, float3 materialColor, Material matData)
{
    float3 lightDir = normalize(-light.direction);
    float3 lightColorIntensity = light.color.rgb * light.intensity;

    return CalculateShading(lightDir, lightColorIntensity, normal, viewDirection, materialColor, matData);
}