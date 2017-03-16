//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

//
// Sampler and texture.
//
Texture2D MaterialTexture : register(t0);
SamplerState MaterialSampler : register(s0);

//
// Shader output.
//
struct VS_OUTPUT
{
    float4 Position : SV_Position;
    float2 TexCoord : TEXCOORD0;
    float3 Normal : NORMAL;
    float4 Color : COLOR;
};

//
// Some constants :D
//
static const float4 AmbientColor = float4(1, 1, 1, 1);
static const float AmbientIntensity = 0.1;


float4 main(VS_OUTPUT input) : SV_TARGET
{
    //
    // Sample texture.
    //
    float4 textureColor = MaterialTexture.Sample(MaterialSampler, input.TexCoord);

    //
    // And apply lighting.
    //
    return textureColor * saturate(input.Color + AmbientColor * AmbientIntensity);
}