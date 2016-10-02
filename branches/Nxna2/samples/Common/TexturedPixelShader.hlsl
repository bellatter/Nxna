struct PixelInput
{
	float4 position : SV_POSITION;
	float2 texCoords: TEXCOORD;
	float4 color: COLOR;
};

Texture2D<float4> Diffuse : register(t0);
sampler DiffuseSampler : register(s0);

#define SAMPLE_TEXTURE(Name, texCoord)  Name.Sample(Name##Sampler, texCoord)

float4 PixelShaderMain(PixelInput input) : SV_TARGET
{
	return SAMPLE_TEXTURE(Diffuse, input.texCoords) * input.color;
}