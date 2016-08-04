struct PixelInput
{
	float4 position : SV_POSITION;
	float4 color: COLOR;
};

float4 PixelShaderMain(PixelInput input) : SV_TARGET
{
	return input.color;
}