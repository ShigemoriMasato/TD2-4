
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
    int32_t isActive;
};
ConstantBuffer<Material> gMaterial : register(b0);

struct DirectionalLight
{
    float4 color;
    float3 direction;
    float intensity;
    float3 cameraWorldPos;
};
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

Texture2D<float32_t4> gTexture[] : register(t8);
SamplerState gSampler : register(s0);

struct PSOutput
{
    float4 color : SV_TARGET0;
};

float32_t3 CalculateShading(float32_t3 lightDirection, float32_t3 lightColor, float32_t3 normal, float32_t3 viewDirection, float32_t3 materialColor, Material matData);
float32_t3 CalculateDirectionalLight(DirectionalLight light, float32_t3 normal, float32_t3 viewDirection, float32_t3 materialColor, Material matData);

PSOutput main(VertexShaderOutput input)
{
    PSOutput output;
    float4 textureColor = gTexture[gMaterial.textureIndex].Sample(gSampler, input.texcoord);
    
    if (gMaterial.isActive)
    {
        // 最終的な色
        float32_t3 finalColor = float32_t3(0.0f, 0.0f, 0.0f);
        
        // ライト計算のための共通データを準備
        float32_t3 normal = normalize(input.normal);
        float32_t3 toEye = normalize(gDirectionalLight.cameraWorldPos - input.worldPosition);
        float32_t3 baseColor = gMaterial.color.rgb * textureColor.rgb;
        
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

float32_t3 CalculateShading(float32_t3 lightDirection, float32_t3 lightColor, float32_t3 normal, float32_t3 viewDirection, float32_t3 materialColor, Material matData)
{
    float NdotL = dot(normal, lightDirection);
    float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
    float32_t3 diffuse = materialColor * lightColor * cos;

    float32_t3 halfVector = normalize(lightDirection + viewDirection);
    float NDotH = dot(normal, halfVector);
    float specularPow = pow(saturate(NDotH), matData.shininess);
    float32_t3 specular = lightColor * specularPow;

    return diffuse + specular;
}

float32_t3 CalculateDirectionalLight(DirectionalLight light, float32_t3 normal, float32_t3 viewDirection, float32_t3 materialColor, Material matData)
{
    float32_t3 lightDir = normalize(-light.direction);
    float32_t3 lightColorIntensity = light.color.rgb * light.intensity;

    return CalculateShading(lightDir, lightColorIntensity, normal, viewDirection, materialColor, matData);
}