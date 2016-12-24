// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
// http://go.microsoft.com/fwlink/?LinkId=248929
// http://create.msdn.com/en-US/education/catalog/sample/stock_effects


Texture2D<float4> Diffuse : register(t0);
sampler DiffuseSampler : register(s0);

Texture2D<float4> Diffuse2 : register(t1);
sampler Diffuse2Sampler : register(s1);

cbuffer Parameters : register(b0)
{
   row_major float4x4 ModelViewProjection;
};


void DualTextureVS(inout float4 color    : COLOR0,
                       inout float2 texCoord : TEXCOORD0,
					   inout float2 texCoord2 : TEXCOORD1,
                       inout float4 position : SV_Position)
{
   position = mul(position, ModelViewProjection);
}

void DualTextureNoColorVS(inout float2 texCoord : TEXCOORD0,
					   inout float2 texCoord2 : TEXCOORD1,
                       inout float4 position : SV_Position)
{
	position = mul(position, ModelViewProjection);
}


float4 DualTexturePS(float4 color    : COLOR0,
                        float2 texCoord : TEXCOORD0,
						float2 texCoord2 : TEXCOORD1) : SV_Target0
{
	float4 color1 = Diffuse.Sample(DiffuseSampler, texCoord);
	float4 color2 = Diffuse2.Sample(Diffuse2Sampler, texCoord2);

	return float4(color1.rgb * color2.rgb * 2.0, color1.a * color2.a) * color;
}

float4 DualTextureNoColorPS(float2 texCoord : TEXCOORD0,
						float2 texCoord2 : TEXCOORD1) : SV_Target0
{
	float4 color1 = Diffuse.Sample(DiffuseSampler, texCoord);
	float4 color2 = Diffuse2.Sample(Diffuse2Sampler, texCoord2);

	return float4(color1.rgb * color2.rgb * 2.0, color1.a * color2.a);
}