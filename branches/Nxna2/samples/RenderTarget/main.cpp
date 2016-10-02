#include "MyNxna.h"
#include "Common/Common.h"
#include <cstdio>

#ifdef _WIN32
#include "../Common/TexturedVertexShader_D3D.h"
#include "../Common/TexturedPixelShader_D3D.h"
#endif
#include "../Common/TexturedShader_glsl.h"

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

	// begin initialization
	Nxna::Graphics::GraphicsDeviceDesc params;
	params.Type = Nxna::Graphics::GraphicsDeviceType::OpenGl41;
	params.ScreenWidth = screenWidth;
	params.ScreenHeight = screenHeight;

	// first, create a window
	WindowInfo window;
	CreateGameWindow(params.ScreenWidth, params.ScreenHeight, false, "Nxna2 Render-to-texture Test", &window);

	// now either create a Direct3D device, or an OpenGL context
#ifdef NXNA_ENABLE_DIRECT3D11
	if (params.Type == Nxna::Graphics::GraphicsDeviceType::Direct3D11)
		CreateDirect3DDevice(&window, params.ScreenWidth, params.ScreenHeight, &params.Direct3D11.Device, &params.Direct3D11.DeviceContext, &params.Direct3D11.RenderTargetView, &params.Direct3D11.DepthStencilView, &params.Direct3D11.SwapChain);
	else
#endif
		CreateOpenGLContext(&window);
	ShowGameWindow(window);

	// the window and device/context have been created, so create our device
	Nxna::Graphics::GraphicsDevice sdevice;
	if (Nxna::Graphics::GraphicsDevice::CreateGraphicsDevice(&params, &sdevice) != Nxna::NxnaResult::Success)
		return -1;

	auto device = &sdevice;

	// the device can give us useful debug info, so hook into that
	device->SetMessageCallback(msgCallback);

	// set the viewport
	Nxna::Graphics::Viewport vp(0, 0, (float)screenWidth, (float)screenHeight);
	device->SetViewport(vp);

	// create the matrices
	Nxna::Matrix projection = Nxna::Matrix::CreatePerspectiveFieldOfView(60.0f * 0.0174533f, screenWidth / (float)screenHeight, 0.5f, 1000.0f);
	Nxna::Matrix view = Nxna::Matrix::CreateLookAt(Nxna::Vector3(0, 0, 10.0f), Nxna::Vector3(0, 0, 0), Nxna::Vector3(0, 1.0f, 0));

	// create a "default" texture
	char pixels[] = { 255, 255, 255, 255 };
	Nxna::Graphics::TextureCreationDesc tDesc = {};
	tDesc.Width = 1;
	tDesc.Height = 1;
	tDesc.InitialData = pixels;
	tDesc.InitialDataByteCount = 4;
	Nxna::Graphics::Texture2D defaultTexture;
	if (device->CreateTexture2D(&tDesc, &defaultTexture) != Nxna::NxnaResult::Success)
	{
		printf("Unable to create default texture\n");
		return -1;
	}

	// create the triangle vertex buffer
	struct vertex
	{
		float X, Y, Z;
		float U, V;
		unsigned int Color;
	};

	vertex verts[] = {
		// triangle verts
		{ -3.0f, -3.0f, 0.0f, 0, 0, NXNA_GET_PACKED_COLOR_RGB_BYTES(0, 0, 255), },
		{ 0.0f, 3.0f, 0.0f, 0, 0, NXNA_GET_PACKED_COLOR_RGB_BYTES(0, 255, 0), },
		{ 3.0f, -3.0f, 0.0f, 0, 0, NXNA_GET_PACKED_COLOR_RGB_BYTES(255, 0, 0), },

		// square verts
		{ -3.0f, -3.0f, 0.0f, 0, 1.0f, NXNA_GET_PACKED_COLOR_RGB_BYTES(255, 255, 255), },
		{ -3.0f, 3.0f, 0.0f, 0, 0, NXNA_GET_PACKED_COLOR_RGB_BYTES(255, 255, 255), },
		{ 3.0f, 3.0f, 0.0f, 1.0f, 0, NXNA_GET_PACKED_COLOR_RGB_BYTES(255, 255, 255), },
		{ 3.0f, -3.0f, 0.0f, 1.0f, 1.0f, NXNA_GET_PACKED_COLOR_RGB_BYTES(255, 255, 255), },
	};

	if (device->GetCaps()->TextureOriginUpperLeft == false)
	{
		verts[3].V = 1.0f - verts[3].V;
		verts[4].V = 1.0f - verts[4].V;
		verts[5].V = 1.0f - verts[5].V;
		verts[6].V = 1.0f - verts[6].V;
	}

	Nxna::Graphics::InputElement inputElements[] = {
		{ 0, Nxna::Graphics::InputElementFormat::Vector3, Nxna::Graphics::InputElementUsage::Position, 0 },
		{ 3 * sizeof(float), Nxna::Graphics::InputElementFormat::Vector2, Nxna::Graphics::InputElementUsage::TextureCoordinate, 0},
		{ 5 * sizeof(float), Nxna::Graphics::InputElementFormat::Color, Nxna::Graphics::InputElementUsage::Color, 0 }
	};

	Nxna::Graphics::VertexBuffer vb;
	Nxna::Graphics::VertexBufferDesc vbDesc = {};
	vbDesc.NumVertices = 7;
	vbDesc.StrideBytes = sizeof(vertex);
	vbDesc.InputElements = inputElements;
	vbDesc.NumInputElements = 3;
	vbDesc.InitialData = verts;
	vbDesc.InitialDataByteCount = sizeof(verts);
	if (device->CreateVertexBuffer(&vbDesc, &vb) != Nxna::NxnaResult::Success)
	{
		printf("Unable to create vertex buffer\n");
		return -1;
	}

	// create the index buffer
	unsigned short indices[] = { 
		0, 1, 2, // triangle
		3, 4, 5, // square
		3, 5, 6
	};
	Nxna::Graphics::IndexBuffer ib;
	Nxna::Graphics::IndexBufferDesc ibDesc = {};
	ibDesc.ElementSize = Nxna::Graphics::IndexElementSize::SixteenBits;
	ibDesc.InitialData = indices;
	ibDesc.InitialDataByteCount = sizeof(indices);
	ibDesc.NumElements = 9;
	if (device->CreateIndexBuffer(&ibDesc, &ib) != Nxna::NxnaResult::Success)
	{
		printf("Unable to create index buffer\n");
		return -1;
	}

	// create the vertex and pixel shaders
	Nxna::Graphics::ShaderBytecode vertexShaderBytecode, pixelShaderBytecode;
#ifdef NXNA_ENABLE_DIRECT3D11
	if (device->GetType() == Nxna::Graphics::GraphicsDeviceType::Direct3D11)
	{
		vertexShaderBytecode.Bytecode = g_VertexShaderMain;
		vertexShaderBytecode.BytecodeLength = sizeof(g_VertexShaderMain);
		pixelShaderBytecode.Bytecode = g_PixelShaderMain;
		pixelShaderBytecode.BytecodeLength = sizeof(g_PixelShaderMain);
	}
	else
#endif
	{
		vertexShaderBytecode.Bytecode = g_vsTexturedMain;
		vertexShaderBytecode.BytecodeLength = sizeof(g_vsTexturedMain);
		pixelShaderBytecode.Bytecode = g_psTexturedMain;
		pixelShaderBytecode.BytecodeLength = sizeof(g_psTexturedMain);
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

	// now that the shaders have been created, put them into a ShaderPipeline
	Nxna::Graphics::ShaderPipeline sp;
	Nxna::Graphics::ShaderPipelineDesc spDesc = {};
	spDesc.VertexShader = &vs;
	spDesc.PixelShader = &ps;
	spDesc.VertexShaderBytecode = vertexShaderBytecode;
	spDesc.VertexElements = inputElements;
	spDesc.NumElements = 3;
	if (device->CreateShaderPipeline(&spDesc, &sp) != Nxna::NxnaResult::Success)
	{
		printf("Unable to create shader pipeline\n");
		return -1;
	}

	// now create a constant buffer so we can send parameters to the shaders
	Nxna::Graphics::ConstantBuffer cb;
	Nxna::Graphics::ConstantBufferDesc cbDesc = {};
	Nxna::Matrix modelViewProjection = view * projection;
	cbDesc.InitialData = modelViewProjection.C;
	cbDesc.ByteCount = sizeof(float) * 16;
	if (device->CreateConstantBuffer(&cbDesc, &cb) != Nxna::NxnaResult::Success)
	{
		printf("Unable to create constant buffer\n");
		return -1;
	}

	// create a blend state using default premultiplied alpha blending
	Nxna::Graphics::BlendStateDesc bd;
	bd.IndependentBlendEnabled = false;
	bd.RenderTarget[0] = NXNA_RENDERTARGETBLENDSTATEDESC_ALPHABLEND;
	Nxna::Graphics::BlendState bs;
	if (device->CreateBlendState(&bd, &bs) != Nxna::NxnaResult::Success)
	{
		printf("Unable to create blend state\n");
		return -1;
	}

	// create a rasterization state using default no-culling
	Nxna::Graphics::RasterizerStateDesc rd = NXNA_RASTERIZERSTATEDESC_CULLNONE;
	Nxna::Graphics::RasterizerState rs;
	if (device->CreateRasterizerState(&rd, &rs) != Nxna::NxnaResult::Success)
	{
		printf("Unable to create rasterizer state\n");
		return -1;
	}

	// create a render target
	Nxna::Graphics::RenderTarget rt;
	Nxna::Graphics::RenderTargetDesc rtDesc = {};
	rtDesc.Width = 128;
	rtDesc.Height = 128;
	rtDesc.DepthFormatType = Nxna::Graphics::DepthFormat::Depth24;
	if (device->CreateRenderTarget(&rtDesc, &rt) != Nxna::NxnaResult::Success)
	{
		printf("Unable to create RenderTarget\n");
		return -1;
	}

	// create a texture to render to
	Nxna::Graphics::Texture2D rtTexture;
	Nxna::Graphics::TextureCreationDesc rtTextureDesc = {};
	rtTextureDesc.Width = 128;
	rtTextureDesc.Height = 128;
	rtTextureDesc.Format = Nxna::Graphics::SurfaceFormat::Color;
	rtTextureDesc.Flags = (unsigned int)Nxna::Graphics::TextureCreationFlags::AllowRenderTargetColorAttachment;
	if (device->CreateTexture2D(&rtTextureDesc, &rtTexture) != Nxna::NxnaResult::Success)
	{
		printf("Unable to create RenderTarget texture\n");
		return -1;
	}

	Nxna::Graphics::RenderTargetColorAttachment rtca;
	Nxna::Graphics::RenderTargetColorAttachmentDesc rtcaDesc = {};
	rtcaDesc.Texture = rtTexture;
	if (device->CreateRenderTargetColorAttachment(&rtcaDesc, &rtca) != Nxna::NxnaResult::Success)
	{
		printf("Unable to create RenderTarget color attachment\n");
		return -1;
	}

	Nxna::Graphics::RenderTargetColorAttachment* colorAttachments[] = { &rtca };
	if (device->SetRenderTargetAttachments(&rt, colorAttachments, 1, nullptr) != Nxna::NxnaResult::Success)
	{
		printf("Unable to attach color to RenderTarget\n");
		return -1;
	}

	// now apply all the state objects we just created
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

		// first, render the triangle to a texture
		device->BindRenderTarget(&rt);
		device->SetViewport(0, 0, 128, 128, 0, 1.0f);

		// clear to black
		device->ClearColor(1.0f, 1.0f, 0, 1.0f);
		device->ClearDepthStencil(true, true, 1.0f, 0);

		device->BindTexture(&defaultTexture, 0);

		// rotate the triangle and send the new transformation matrix to the constant buffer
		Nxna::Matrix world = Nxna::Matrix::CreateRotationY(rotation);
		Nxna::Matrix worldViewProjection = world * modelViewProjection;
		device->UpdateConstantBuffer(cb, worldViewProjection.C, 16 * sizeof(float));

		// draw the triangle
		device->DrawIndexedPrimitives(Nxna::Graphics::PrimitiveType::TriangleList, 0, 0, 3, 0, 1);

		// restore the default render target
		device->BindRenderTarget(nullptr);
		device->SetViewport(vp);

		// draw the cube
		
		// clear to black
		device->ClearColor(0, 0, 0, 0);
		device->ClearDepthStencil(true, true, 1.0f, 0);

		device->BindTexture(&rtTexture, 0);

		// rotate the triangle and send the new transformation matrix to the constant buffer
		world = Nxna::Matrix::CreateRotationY(rotation);
		worldViewProjection = world * modelViewProjection;
		device->UpdateConstantBuffer(cb, worldViewProjection.C, 16 * sizeof(float));

		// draw the triangle
		device->DrawIndexedPrimitives(Nxna::Graphics::PrimitiveType::TriangleList, 0, 0, 4, 3, 2);

		// show the results
		device->Present();
		Present(window);

		// increase the rotation
		rotation += 0.1f;
	}

	device->SetShaderPipeline(nullptr);

	device->DestroyRasterizerState(&rs);
	device->DestroyBlendState(&bs);
	device->DestroyConstantBuffer(&cb);
	device->DestroyShaderPipeline(&sp);
	device->DestroyIndexBuffer(ib);
	device->DestroyVertexBuffer(vb);
	Nxna::Graphics::GraphicsDevice::DestroyGraphicsDevice(device);
	DestroyGameWindow(window);

	return 0;
}

#ifdef _WIN32
#include "../Common/Win32.cpp"
#else
//#include "../Common/X.cpp"
#endif

#define NXNA2_IMPLEMENTATION
#include "MyNxna.h"

