cbuffer Matrix : register(b0)
{
	row_major matrix worldViewProjection;
};

struct VertexInput
{
	float4 position : SV_POSITION; 
	float4 color : COLOR0;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float4 color: COLOR0;
};

PixelInput VertexShaderMain(VertexInput input)
{
	PixelInput output;
	
	output.position = mul(input.position, worldViewProjection);
	output.color = input.color;
	
	return output;
}