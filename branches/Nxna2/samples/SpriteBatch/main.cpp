#include "MyNxna.h"
#include "Common/Common.h"
#include "smily.h"

#include <cstdio>
#include <vector>

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

const int BATCH_SIZE = 100;

class MySpriteBatch
{
	std::vector<Nxna::Graphics::SpriteBatchSprite> m_sprites;
	Nxna::Graphics::GraphicsDevice* m_device;
	Nxna::Graphics::VertexBuffer m_vertexBuffer;
	static Nxna::Graphics::IndexBuffer m_indexBuffer;
	static Nxna::Graphics::ShaderPipeline m_shaderPipeline;
	static Nxna::Graphics::ConstantBuffer m_constantBuffer;
	static Nxna::Graphics::BlendState m_blendState;
	static Nxna::Graphics::RasterizerState m_rasterState;
	static Nxna::Graphics::DepthStencilState m_depthState;
	static Nxna::Graphics::SamplerState m_samplerState;
	static unsigned int m_stride;
	static bool m_staticDataInitialized;
public:

	static int Init(Nxna::Graphics::GraphicsDevice* device, MySpriteBatch* result)
	{
		new(&result->m_sprites) std::vector<Nxna::Graphics::SpriteBatchSprite>();

		result->m_device = device;

		Nxna::Graphics::InputElement elements[3];
		Nxna::Graphics::SpriteBatch::SetupVertexElements(elements, &m_stride);

		// create the vertex buffer
		Nxna::Graphics::VertexBufferDesc vbDesc = {};
		vbDesc.BufferUsage = Nxna::Graphics::Usage::Dynamic;
		vbDesc.NumVertices = BATCH_SIZE * 4;
		vbDesc.StrideBytes = m_stride;
		vbDesc.InputElements = elements;
		vbDesc.NumInputElements = 3;
		vbDesc.InitialData = nullptr;
		vbDesc.InitialDataByteCount = 0;
		if (device->CreateVertexBuffer(&vbDesc, &result->m_vertexBuffer) != Nxna::NxnaResult::Success)
		{
			printf("Unable to create vertex buffer\n");
			return -1;
		}

		if (m_staticDataInitialized == false)
		{
			m_staticDataInitialized = true;

			// create the index buffer
			unsigned short indices[BATCH_SIZE * 6];
			Nxna::Graphics::SpriteBatch::FillIndexBuffer(indices, BATCH_SIZE * 6);
			Nxna::Graphics::IndexBufferDesc ibDesc = {};
			ibDesc.ElementSize = Nxna::Graphics::IndexElementSize::SixteenBits;
			ibDesc.InitialData = indices;
			ibDesc.InitialDataByteCount = sizeof(unsigned short) * BATCH_SIZE * 6;
			ibDesc.NumElements = BATCH_SIZE * 6;
			if (device->CreateIndexBuffer(&ibDesc, &m_indexBuffer) != Nxna::NxnaResult::Success)
			{
				printf("Unable to create index buffer\n");
				return -1;
			}

			// create the vertex and pixel shaders
			Nxna::Graphics::ShaderBytecode vertexShaderBytecode, pixelShaderBytecode;
			Nxna::Graphics::SpriteBatch::GetShaderBytecode(device, &vertexShaderBytecode, &pixelShaderBytecode);

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
			Nxna::Graphics::ShaderPipelineDesc spDesc = {};
			spDesc.VertexShader = &vs;
			spDesc.PixelShader = &ps;
			spDesc.VertexShaderBytecode = vertexShaderBytecode;
			spDesc.VertexElements = elements;
			spDesc.NumElements = 3;
			if (device->CreateShaderPipeline(&spDesc, &m_shaderPipeline) != Nxna::NxnaResult::Success)
			{
				printf("Unable to create shader pipeline\n");
				return -1;
			}

			// now create a constant buffer so we can send parameters to the shaders
			unsigned char constantBufferData[16 * sizeof(float)];
			Nxna::Graphics::SpriteBatch::SetupConstantBuffer(device->GetViewport(), constantBufferData, 16 * sizeof(float));
			Nxna::Graphics::ConstantBufferDesc cbDesc = {};
			cbDesc.InitialData = constantBufferData;
			cbDesc.ByteCount = sizeof(float) * 16;
			if (device->CreateConstantBuffer(&cbDesc, &m_constantBuffer) != Nxna::NxnaResult::Success)
			{
				printf("Unable to create constant buffer\n");
				return -1;
			}

			// create a blend state using default premultiplied alpha blending
			Nxna::Graphics::BlendStateDesc bd;
			bd.IndependentBlendEnabled = false;
			bd.RenderTarget[0] = NXNA_RENDERTARGETBLENDSTATEDESC_ALPHABLEND;
			if (device->CreateBlendState(&bd, &m_blendState) != Nxna::NxnaResult::Success)
			{
				printf("Unable to create blend state\n");
				return -1;
			}

			// create a rasterization state using default no-culling
			Nxna::Graphics::RasterizerStateDesc rd = NXNA_RASTERIZERSTATEDESC_CULLNONE;
			if (device->CreateRasterizerState(&rd, &m_rasterState) != Nxna::NxnaResult::Success)
			{
				printf("Unable to create rasterizer state\n");
				return -1;
			}

			// create a depth/stencil state using read-only
			Nxna::Graphics::DepthStencilStateDesc dd = NXNA_DEPTHSTENCIL_DEPTHREAD;
			if (device->CreateDepthStencilState(&dd, &m_depthState) != Nxna::NxnaResult::Success)
			{
				printf("Unable to create depth/stencil state\n");
				return -1;
			}

			// create a point-filtering sampler state
			Nxna::Graphics::SamplerStateDesc sd = NXNA_SAMPLERSTATEDESC_POINTCLAMP;
			if (device->CreateSamplerState(&sd, &m_samplerState) != Nxna::NxnaResult::Success)
			{
				printf("Unable to create sampler state\n");
				return -1;
			}
		}

		return 0;
	}

	static void Destroy(MySpriteBatch* spriteBatch)
	{
		spriteBatch->m_device->DestroyVertexBuffer(spriteBatch->m_vertexBuffer);
	}

	void Begin()
	{
		m_sprites.clear();
	}

	void Draw(Nxna::Graphics::Texture2D* texture, int textureWidth, int textureHeight, float x, float y, float width, float height, Nxna::Color color)
	{
		Nxna::Graphics::SpriteBatchSprite sprite;
		Nxna::Graphics::SpriteBatch::WriteSprite(&sprite, texture, textureWidth, textureHeight, x, y, width, height, color);

		m_sprites.push_back(sprite);
	}

	void End()
	{
		if (m_sprites.empty()) return;

		// TODO: sort the sprites by texture

		// set states
		m_device->SetBlendState(&m_blendState);
		m_device->SetRasterizerState(&m_rasterState);
		m_device->SetDepthStencilState(&m_depthState);
		m_device->SetConstantBuffer(m_constantBuffer, 0);
		m_device->SetShaderPipeline(&m_shaderPipeline);
		m_device->SetSamplerState(0, &m_samplerState);
		m_device->SetVertexBuffer(m_vertexBuffer, 0, m_stride);
		m_device->SetIndices(m_indexBuffer);

		unsigned int spritesDrawn = 0;
		while (true)
		{
			unsigned int textureChanges[10];
			unsigned int numTextureChanges = 10;

			auto vbp = m_device->MapBuffer(m_vertexBuffer, Nxna::Graphics::MapType::WriteDiscard);
			unsigned int spritesAdded = Nxna::Graphics::SpriteBatch::FillVertexBuffer(&m_sprites[0] + spritesDrawn, nullptr, m_sprites.size() - spritesDrawn, vbp, BATCH_SIZE * 4 * sizeof(float), textureChanges, &numTextureChanges);
			m_device->UnmapBuffer(m_vertexBuffer);

			unsigned int currentSprite = 0;
			for (unsigned int i = 0; i < numTextureChanges; i++)
			{
				m_device->BindTexture(&m_sprites[spritesDrawn + currentSprite].Texture, 0);

				m_device->DrawIndexedPrimitives(Nxna::Graphics::PrimitiveType::TriangleList, 0, 0, BATCH_SIZE * 4, currentSprite * 6, textureChanges[i] * 2);
				currentSprite += textureChanges[i];
			}
			spritesDrawn += spritesAdded;

			if (spritesDrawn == m_sprites.size()) 
				break;
		}

		m_sprites.clear();
	}
};

Nxna::Graphics::IndexBuffer MySpriteBatch::m_indexBuffer;
Nxna::Graphics::ShaderPipeline MySpriteBatch::m_shaderPipeline;
Nxna::Graphics::ConstantBuffer MySpriteBatch::m_constantBuffer;
Nxna::Graphics::BlendState MySpriteBatch::m_blendState;
Nxna::Graphics::RasterizerState MySpriteBatch::m_rasterState;
Nxna::Graphics::DepthStencilState MySpriteBatch::m_depthState;
Nxna::Graphics::SamplerState MySpriteBatch::m_samplerState;
unsigned int MySpriteBatch::m_stride;
bool MySpriteBatch::m_staticDataInitialized = false;

Nxna::NxnaResult createTexture(Nxna::Graphics::GraphicsDevice* device, Nxna::Graphics::Texture2D* texture)
{
	unsigned char pixels[width * height * 4];
	unsigned char *pixel = pixels;
	unsigned char* data = (unsigned char*)header_data;

	for (int i = 0; i < width * height; i++)
	{
		HEADER_PIXEL(data, pixel);
		pixel[3] = 255;
		pixel += 4;
	}

	Nxna::Graphics::TextureCreationDesc desc = {};
	desc.Width = width;
	desc.Height = height;
	desc.InitialData = pixels;
	desc.InitialDataByteCount = width * height * 3;
	desc.Format = Nxna::Graphics::SurfaceFormat::Color;
	desc.MipLevels = 1;
	return device->CreateTexture2D(&desc, texture);
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
	CreateGameWindow(params.ScreenWidth, params.ScreenHeight, false, "Nxna2 SpriteBatch Test", &window);

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
	
	// create the texture
	Nxna::Graphics::Texture2D smily;
	if (createTexture(device, &smily) != Nxna::NxnaResult::Success)
		return -1;

	MySpriteBatch sb;
	MySpriteBatch::Init(device, &sb);

	const float size = 100.0f;
	const float vel = 0.10f;
	float x = 0, y = 0;
	float vx = vel, vy = vel;

	float rotation = 0;
	while (g_quitReceived == false)
	{
		HandleMessages(window);

		// bounce around
		{
			x += vx;
			y += vy;

			if (x + size > vp.Width && vx > 0)
			{
				vx = -vel;
				x = vp.Width - size;
			}
			else if (x < 0 && vx < 0)
			{
				vx = vel;
				x = 0;
			}

			if (y + size > vp.Height && vy > 0)
			{
				vy = -vel;
				y = vp.Height - size;
			}
			else if (y < 0 && vy < 0)
			{
				vy = vel;
				y = 0;
			}
		}

		// clear to black
		device->ClearColor(0, 0, 0, 0);
		device->ClearDepthStencil(true, true, 1.0f, 0);

		sb.Begin();

		sb.Draw(&smily, width, height, vp.Width / 2 - 200, vp.Height / 2 - 200, 400, 400, Nxna::Color(255, 255, 255));
		sb.Draw(&smily, width, height, x, y, 100, 100, Nxna::Color(128, 128, 128, 128));

		sb.End();

		// show the results
		device->Present();
		Present(window);
	}

	device->SetShaderPipeline(nullptr);

	MySpriteBatch::Destroy(&sb);

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

