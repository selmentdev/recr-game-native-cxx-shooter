//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

//
// Camera data.
//
cbuffer CameraData : register(b0)
{
    float4x4 CameraData_View;
    float4x4 CameraData_Projection;
};

//
// Object data.
//
// Good candidate for instancing too.
//
cbuffer ObjectData : register(b1)
{
    float4x4 ObjectData_World;
    float4x4 ObjectData_InverseWorld;
};

//
// Input and output.
//
struct VS_INPUT
{
    float3 Position : SV_Position;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 Position : SV_Position;
    float2 TexCoord : TEXCOORD0;
    float3 Normal : NORMAL;
    float4 Color : COLOR;
};

//
// Some predefined lighting.
//
static const float3 DiffuseLightDirection = normalize(float3(1.0F, 0.5F, -1.0F));
static const float4 DiffuseColor = float4(1, 1, 1, 1);
static const float DiffuseIntensity = 1.0;

VS_OUTPUT main(VS_INPUT input)
{
    //
    // Expand position to homogenous space.
    //
    float4 position = float4(input.Position, 1.0F);
    
    //
    // Compute WV + WVP matrices.
    //
    float4x4 worldView = mul(CameraData_View, ObjectData_World);
    float4x4 worldViewProjection = mul(CameraData_Projection, worldView);

    //
    // Transform positon.
    //
    position = mul(worldViewProjection, position);

    //
    // Pass that position and texcoord to PS.
    //
    VS_OUTPUT output;
    output.Position = position;
    output.TexCoord = input.TexCoord;

    //
    // Compute normal from local space to world space.
    //
    output.Normal = mul(input.Normal, (float3x3)ObjectData_InverseWorld);

    //
    // Compute light intensity based on normal and light direction.
    //
    float lightIntensity = dot(output.Normal, DiffuseLightDirection);

    //
    // And saturate that by diffuse color and light intensity. Just per vertex lighting.
    //
    output.Color = saturate(DiffuseColor * DiffuseIntensity * lightIntensity);
    return output;
}