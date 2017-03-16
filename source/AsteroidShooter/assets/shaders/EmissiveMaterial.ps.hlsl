//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

//
// Emmisive material pixel shader.
//
// Fake. Just sample texture at texcoord and it's done :)
//

cbuffer MaterialData : register(b0)
{
    float4 MaterialData_Color;
};

//
// Texture and sampler.
//
Texture2D MaterialTexture : register(t0);
SamplerState MaterialSampler : register(s0);

//
// Input for pixel shader (aka VS_OUTPUT :) )
//
struct VS_OUTPUT
{
    float4 Position : SV_Position;
    float2 TexCoord : TEXCOORD0;
};


float4 main(VS_OUTPUT input) : SV_TARGET
{
    //
    // Well. Sample texture and return.
    //
    float4 textureColor = MaterialTexture.Sample(MaterialSampler, input.TexCoord);
    return textureColor;
}