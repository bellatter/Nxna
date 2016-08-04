#include "MyNxna.h"
#include "Common/Common.h"
#include <cstdio>

#ifdef _WIN32
#include "../Common/BasicVertexShader_D3D.h"
#include "../Common/BasicPixelShader_D3D.h"
#endif
#include "../Common/BasicShader_glsl.h"

extern bool g_quitReceived;

// force nvidia over intel integrated hardware
#ifdef _WIN32
extern "C"
{
	//_declspec(dllexport) unsigned int NvOptimusEnablement = 0x00000001;
}
#endif

void msgCallback(Nxna::Graphics::GraphicsDeviceDebugMessage msg)
{
	printf("%s\n", msg.Message);
}

int main(int argc, char** argv)
{
	const int screenWidth = 640;
	const int screenHeight = 480;

	Nxna::Graphics::GraphicsDeviceCreationParams params;
	params.Type = Nxna::Graphics::GraphicsDeviceType::OpenGl41;
	params.ScreenWidth = screenWidth;
	params.ScreenHeight = screenHeight;

	WindowInfo window;
	CreateGameWindow(params.ScreenWidth, params.ScreenHeight, false, "Nxna2 Triangle Test", &window);
	if (params.Type == Nxna::Graphics::GraphicsDeviceType::Direct3D11)
		CreateDirect3DDevice(&window, params.ScreenWidth, params.ScreenHeight, &params.Direct3D11.Device, &params.Direct3D11.DeviceContext, &params.Direct3D11.RenderTargetView, &params.Direct3D11.SwapChain);
	else
		CreateOpenGLContext(&window);
	ShowGameWindow(window);

	auto device = new Nxna::Graphics::GraphicsDevice(&params);

	device->SetMessageCallback(msgCallback);
	Nxna::Graphics::Viewport vp(0, 0, screenWidth, screenHeight);
	device->SetViewport(vp);


	// create the matrices
	Nxna::Matrix projection = Nxna::Matrix::CreatePerspectiveFieldOfView(60.0f * 0.0174533f, screenWidth / (float)screenHeight, 0.5f, 1000.0f);
	Nxna::Matrix view = Nxna::Matrix::CreateLookAt(Nxna::Vector3(0, 0, 10.0f), Nxna::Vector3(0, 0, 0), Nxna::Vector3(0, 1.0f, 0));

	// create the triangle vertex buffer
	struct vertex
	{
		float X, Y, Z;
		unsigned int Color;
	};

	vertex verts[] = {
			{ -3.0f, -3.0f, 0.0f, NXNA_GET_PACKED_COLOR_RGB_BYTES(0, 0, 255), },
			{ 0.0f, 3.0f, 0.0f, NXNA_GET_PACKED_COLOR_RGB_BYTES(0, 255, 0), },
			{ 3.0f, -3.0f, 0.0f, NXNA_GET_PACKED_COLOR_RGB_BYTES(255, 0, 0), } 
	};

	Nxna::Graphics::InputElement inputElements[] = {
			{ 0, Nxna::Graphics::InputElementFormat::Vector3, Nxna::Graphics::InputElementUsage::Position, 0},
			{ 3 * sizeof(float), Nxna::Graphics::InputElementFormat::Color, Nxna::Graphics::InputElementUsage::Color, 0 }
	};

	Nxna::Graphics::VertexBuffer vb;
	Nxna::Graphics::VertexBufferDesc vbDesc = {};
	vbDesc.NumVertices = 3;
	vbDesc.StrideBytes = sizeof(vertex);
	vbDesc.InputElements = inputElements;
	vbDesc.NumInputElements = 2;
	vbDesc.InitialData = verts;
	vbDesc.InitialDataByteCount = sizeof(vertex) * 3;
	if (device->CreateVertexBuffer(&vbDesc, &vb) != Nxna::NxnaResult::Success)
	{
		printf("Unable to create vertex buffer\n");
		return -1;
	}

	unsigned short indices[] = { 0, 1, 2 };
	Nxna::Graphics::IndexBuffer ib;
	Nxna::Graphics::IndexBufferDesc ibDesc = {};
	ibDesc.ElementSize = Nxna::Graphics::IndexElementSize::SixteenBits;
	ibDesc.InitialData = indices;
	ibDesc.InitialDataByteCount = sizeof(indices);
	ibDesc.NumElements = 3;
	if (device->CreateIndexBuffer(&ibDesc, &ib) != Nxna::NxnaResult::Success)
	{
		printf("Unable to create index buffer\n");
		return -1;
	}

	Nxna::Graphics::ShaderBytecode vertexShaderBytecode, pixelShaderBytecode;

	if (device->GetType() == Nxna::Graphics::GraphicsDeviceType::Direct3D11)
	{
		vertexShaderBytecode.Bytecode = g_VertexShaderMain;
		vertexShaderBytecode.BytecodeLength = sizeof(g_VertexShaderMain);
		pixelShaderBytecode.Bytecode = g_PixelShaderMain;
		pixelShaderBytecode.BytecodeLength = sizeof(g_PixelShaderMain);
	}
	else
	{
		vertexShaderBytecode.Bytecode = g_vsMain;
		vertexShaderBytecode.BytecodeLength = sizeof(g_vsMain);
		pixelShaderBytecode.Bytecode = g_psMain;
		pixelShaderBytecode.BytecodeLength = sizeof(g_psMain);
	}

	Nxna::Graphics::Shader vs, ps;
	if (device->CreateShader(Nxna::Graphics::ShaderType::Vertex, vertexShaderBytecode.Bytecode, vertexShaderBytecode.BytecodeLength, &vs) != Nxna::NxnaResult::Success)
	{
		printf("Unable to create vertex shader\n");
		return -1;
	}
	if (device->CreateShader(Nxna::Graphics::ShaderType::Pixel, pixelShaderBytecode.Bytecode, pixelShaderBytecode.BytecodeLength, &ps) != Nxna::NxnaResult::Success)
	{
		printf("Unable to create pixel shader\n");
		return -1;
	}
	Nxna::Graphics::ShaderPipeline sp;
	Nxna::Graphics::ShaderPipelineDesc spDesc = {};
	spDesc.VertexShader = &vs;
	spDesc.PixelShader = &ps;
	spDesc.VertexShaderBytecode = vertexShaderBytecode;
	spDesc.VertexElements = inputElements;
	spDesc.NumElements = 2;
	if (device->CreateShaderPipeline(&spDesc, &sp) != Nxna::NxnaResult::Success)
	{
		printf("Unable to create shader pipeline\n");
		return -1;
	}

	Nxna::Graphics::ConstantBuffer cb;
	Nxna::Graphics::ConstantBufferDesc cbDesc = {};
	Nxna::Matrix modelViewProjection = view * projection;
	//Nxna::Matrix modelViewProjection = projection * view;
	cbDesc.InitialData = modelViewProjection.C;
	cbDesc.ByteCount = sizeof(float) * 16;
	if (device->CreateConstantBuffer(&cbDesc, &cb) != Nxna::NxnaResult::Success)
	{
		printf("Unable to create constant buffer\n");
		return -1;
	}

	Nxna::Graphics::BlendStateDesc bd;
	bd.IndependentBlendEnabled = false;
	bd.RenderTarget[0] = NXNA_RENDERTARGETBLENDSTATEDESC_ALPHABLEND;
	Nxna::Graphics::BlendState bs;
	if (device->CreateBlendState(&bd, &bs) != Nxna::NxnaResult::Success)
	{
		printf("Unable to create blend state\n");
		return -1;
	}

	Nxna::Graphics::RasterizerStateDesc rd = NXNA_RASTERIZERSTATEDESC_CULLNONE;
	Nxna::Graphics::RasterizerState rs;
 	if (device->CreateRasterizerState(&rd, &rs) != Nxna::NxnaResult::Success)
	{
		printf("Unable to create rasterizer state\n");
		return -1;
	}

	device->SetConstantBuffer(cb, 0);
	device->SetShaderPipeline(&sp);
	device->SetBlendState(&bs);
	device->SetRasterizerState(&rs);
	device->SetVertexBuffer(vb, 0, sizeof(vertex));
	device->SetIndices(ib);

	float rotation = 0;
	while (g_quitReceived == false)
	{
		HandleMessages(window);

		device->Clear(0, 0, 0, 0);

		Nxna::Matrix world = Nxna::Matrix::CreateRotationY(rotation);
		Nxna::Matrix worldViewProjection = world * modelViewProjection;
		device->UpdateConstantBuffer(cb, worldViewProjection.C, 16 * sizeof(float));

		device->DrawIndexedPrimitives(Nxna::Graphics::PrimitiveType::TriangleList, 0, 0, 3, 0, 1);

		device->Present();
		Present(window);

		rotation += 0.1f;
	}

	return 0;
}

#define NXNA2_IMPLEMENTATION
#include "MyNxna.h"

#include "../Common/Win32.cpp"