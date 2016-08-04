#include "GraphicsDevice.h"
#include "OpenGL.h"
#include "PipelineState.h"
#include <cstdio>
#include <cstring>

namespace Nxna
{
namespace Graphics
{
	static const int g_numDeviceTypes = 4;
	GraphicsDeviceMessageCallback g_callbacks[g_numDeviceTypes];

	void GLEWAPIENTRY glDebugOutputCallback(GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar* message,
		const void* userParam)
	{
		// TODO: in the future there may be multiple types of OpenGL contexts, so make this handle that

		if (g_callbacks[(int)GraphicsDeviceType::OpenGl41] != nullptr)
		{
			GraphicsDeviceDebugMessage msg;
			msg.DeviceType = GraphicsDeviceType::OpenGl41;
			msg.Message = message;

			g_callbacks[(int)GraphicsDeviceType::OpenGl41](msg);
		}
	}

	GraphicsDevice::GraphicsDevice(const GraphicsDeviceCreationParams* params)
	{
		m_type = params->Type;
		m_screenWidth = params->ScreenWidth;
		m_screenHeight = params->ScreenHeight;

		m_shaderPipeline = nullptr;
		m_blendState = nullptr;
		m_rasterizerState = nullptr;

		switch (params->Type)
		{
		case GraphicsDeviceType::OpenGl41:
			
			int major, minor;
			glGetIntegerv(GL_MAJOR_VERSION, &major);
			glGetIntegerv(GL_MINOR_VERSION, &minor);

			if (major < 4 || (major == 4 && minor < 1))
			{
				// warning! Invalid context! What should we do? Throw an exception?
				;
			}

			OpenGL::LoadGLExtensions(4, 1);

			break;
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
			m_d3d11State.Device = params->Direct3D11.Device;
			m_d3d11State.Context = params->Direct3D11.DeviceContext;
			m_d3d11State.RenderTargetView = params->Direct3D11.RenderTargetView;
			m_d3d11State.SwapChain = params->Direct3D11.SwapChain;
			break;
#endif
		}
	}

	void GraphicsDevice::SetMessageCallback(GraphicsDeviceMessageCallback callback)
	{
		g_callbacks[(int)m_type] = callback;

		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
			// TODO
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			if (callback == nullptr)
			{
				glDisable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			}
			else
			{
				glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
				glDebugMessageCallback(glDebugOutputCallback, nullptr);
			}
		}
		break;
		}

#ifndef NDEBUG
		static_assert((int)GraphicsDeviceType::LAST == g_numDeviceTypes, "m_numDeviceTypes is incorrect");
#endif
	}

	void GraphicsDevice::SetViewport(int x, int y, int width, int height)
	{
		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			D3D11_VIEWPORT vp;
			vp.TopLeftX = (float)x;
			vp.TopLeftY = (float)y;
			vp.Width = (float)width;
			vp.Height = (float)height;
			vp.MinDepth = 0;
			vp.MaxDepth = 1.0f;

			m_d3d11State.Context->RSSetViewports(1, &vp);
		}
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			// OpenGL stores the bottom-left corner, but XNA
			// stores the upper-left corner, so we have to convert.
			y = m_screenHeight - (height + y);

			glViewport(x, y, width, height);
		}
		break;
		}
	}

	void GraphicsDevice::SetViewport(Viewport viewport)
	{
		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			D3D11_VIEWPORT vp;
			vp.TopLeftX = (float)viewport.X;
			vp.TopLeftY = (float)viewport.Y;
			vp.Width = (float)viewport.Width;
			vp.Height = (float)viewport.Height;
			vp.MinDepth = 0;
			vp.MaxDepth = 1.0f;

			m_d3d11State.Context->RSSetViewports(1, &vp);
		}
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{

			// OpenGL stores the bottom-left corner, but XNA
			// stores the upper-left corner, so we have to convert.
			int y = m_screenHeight - (viewport.Height + viewport.Y);

			glViewport(viewport.X, y, viewport.Width, viewport.Height);
		}
		}
	}

#ifdef _WIN32
#define NXNA_SET_ERROR_DETAILS(api, desc) { m_errorDetails.Filename = __FILE__; m_errorDetails.LineNumber = __LINE__; m_errorDetails.APIErrorCode = api; \
	strncpy_s(m_errorDetails.ErrorDescription, desc, 255); m_errorDetails.ErrorDescription[255] = 0; }
#else
#define NXNA_SET_ERROR_DETAILS(api, desc) { m_errorDetails.Filename = __FILE__; m_errorDetails.LineNumber = __LINE__; m_errorDetails.APIErrorCode = api; \
	strncpy(m_errorDetails.ErrorDescription, desc, 255); m_errorDetails.ErrorDescription[255] = 0; }
#endif

	NxnaResult GraphicsDevice::CreateShader(ShaderType type, const void* bytecode, int bytecodeLength, Shader* result)
	{
		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			long r;
			switch(type)
			{
			case ShaderType::Vertex:
				r = m_d3d11State.Device->CreateVertexShader(bytecode, bytecodeLength, nullptr, (ID3D11VertexShader**)&result->Direct3D11.Ptr);
				break;
			case ShaderType::Pixel:
				r = m_d3d11State.Device->CreatePixelShader(bytecode, bytecodeLength, nullptr, (ID3D11PixelShader**)&result->Direct3D11.Ptr);
				break;
			default:
				NXNA_SET_ERROR_DETAILS(0, "Unknown shader type. Probably a bug within Nxna!");
				return NxnaResult::UnknownError;
			}

			if (FAILED(r))
			{
				NXNA_SET_ERROR_DETAILS(r, "Direct3D returned an error when creating the shader");
				return NxnaResult::UnknownError;
			}
		}
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			GLenum glType;
			switch (type)
			{
			case ShaderType::Vertex:
				glType = GL_VERTEX_SHADER;
				break;
			case ShaderType::Pixel:
				glType = GL_FRAGMENT_SHADER;
				break;
			default:
				NXNA_SET_ERROR_DETAILS(0, "Unknown shader type. Probably a bug within Nxna!");
				return NxnaResult::UnknownError;
			}

			const GLchar* buffer[] = { (char*)bytecode };
			auto s = glCreateShaderProgramv(glType, 1, buffer);
			if (s == 0)
			{
				NXNA_SET_ERROR_DETAILS(0, "glCreateShaderProgamv() was unable to create a new shader program");
				return NxnaResult::UnknownError; // unknown error :(
			}

			int logLength;
			char logBuffer[512];
			glGetProgramInfoLog(s, 511, &logLength, logBuffer);
			logBuffer[511] = 0;

			GLint isLinked = 0;
			glGetProgramiv(s, GL_LINK_STATUS, &isLinked);
			if (isLinked == 0)
			{
				glDeleteProgram(s);
				NXNA_SET_ERROR_DETAILS(0, logBuffer);
				return NxnaResult::UnknownError;
			}

			result->OpenGL.Handle = s;
		}
			break;
		}

		return NxnaResult::Success;
	}

	void GraphicsDevice::DestroyShader(Shader* shader)
	{
		NXNA_VALIDATION_ASSERT(shader != nullptr, "shader cannot be null");

		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			((IUnknown*)shader->Direct3D11.Ptr)->Release();
			shader->Direct3D11.Ptr = nullptr;
		}
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			glDeleteProgram(shader->OpenGL.Handle);
		}
			break;
		}
	}

	NxnaResult GraphicsDevice::CreateTexture2D(const TextureCreationDesc* desc, Texture2D* result)
	{
		NXNA_VALIDATION_ASSERT(desc != nullptr, "desc cannot be null");

		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			HRESULT r;
			D3D11_TEXTURE2D_DESC dtdesc;
			ZeroMemory(&dtdesc, sizeof(D3D11_TEXTURE2D_DESC));

			dtdesc.ArraySize = 1;
			dtdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			dtdesc.Width = desc->Width;
			dtdesc.Height = desc->Height;
			dtdesc.MipLevels = desc->MipLevels;
			dtdesc.SampleDesc.Count = 1;
			dtdesc.SampleDesc.Quality = 0;
			dtdesc.Usage = D3D11_USAGE_DEFAULT;
			dtdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			dtdesc.CPUAccessFlags = 0;
			dtdesc.MiscFlags = 0;

			if (desc->InitialData != nullptr)
			{
				D3D11_SUBRESOURCE_DATA initData;
				initData.pSysMem = desc->InitialData;
				if (desc->Format == SurfaceFormat::Color)
					initData.SysMemPitch = 4 * desc->Width;
				else
				{
					int numBlocks = desc->Width / 4;
					if (numBlocks == 0) numBlocks = 1;
					initData.SysMemPitch = numBlocks * (desc->Format == SurfaceFormat::Dxt1 ? 8 : 16);
				}

				r = m_d3d11State.Device->CreateTexture2D(&dtdesc, &initData, &result->Direct3D11.m_texture);
				if (FAILED(r) || result->Direct3D11.m_texture == nullptr)
				{
					NXNA_SET_ERROR_DETAILS(r, "CreateTexture2D() failed");
					return NxnaResult::UnknownError;
				}
			}
			else
			{
				r = m_d3d11State.Device->CreateTexture2D(&dtdesc, nullptr, &result->Direct3D11.m_texture);
				if (FAILED(r) || result->Direct3D11.m_texture == nullptr)
				{
					NXNA_SET_ERROR_DETAILS(r, "CreateTexture2D failed");
					return NxnaResult::UnknownError;
				}
			}

			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			ZeroMemory(&srvDesc, sizeof(srvDesc));
			srvDesc.Format = dtdesc.Format;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = desc->MipLevels;

			r = m_d3d11State.Device->CreateShaderResourceView(result->Direct3D11.m_texture, &srvDesc, &result->Direct3D11.m_shaderResourceView);
			if (FAILED(r))
			{
				NXNA_SET_ERROR_DETAILS(r, "CreateShaderResourceView() failed");
				return NxnaResult::UnknownError;
			}
		}
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			int mipLevels = desc->MipLevels;
			if (mipLevels == 0)
				mipLevels = 1;


			glGenTextures(1, &result->OpenGL.Handle);
			glBindTexture(GL_TEXTURE_2D, result->OpenGL.Handle);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipLevels - 1);

			if (glTexStorage2D)
			{
				glTexStorage2D(GL_TEXTURE_2D, desc->MipLevels, GL_RGBA8, desc->Width, desc->Height);
				if (desc->InitialData)
					glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, desc->Width, desc->Height, GL_RGBA, GL_UNSIGNED_BYTE, desc->InitialData);
			}
			else
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, desc->Width, desc->Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, desc->InitialData);
			}
		}
			break;
		}

		return NxnaResult::Success;
	}

	void GraphicsDevice::BindTexture(Texture2D* texture, int textureUnit)
	{
		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			m_d3d11State.Context->PSSetShaderResources(textureUnit, 1, &texture->Direct3D11.m_shaderResourceView);
		}
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
			glActiveTexture(GL_TEXTURE0 + textureUnit);
			glBindTexture(GL_TEXTURE_2D, texture->OpenGL.Handle);
			break;
		}
	}

	void GraphicsDevice::DestroyTexture2D(Texture2D* texture)
	{
		// TODO
	}

	NxnaResult GraphicsDevice::CreateShaderPipeline(const ShaderPipelineDesc* desc, ShaderPipeline* result)
	{
		NXNA_VALIDATION_ASSERT(desc != nullptr, "desc cannot be null");
		NXNA_VALIDATION_ASSERT(result != nullptr, "result cannot be null");

		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			static D3D11_INPUT_ELEMENT_DESC iedesc[16];
			InputElement* e = desc->VertexElements;

			for (int i = 0; i < desc->NumElements; i++)
			{
				switch (e[i].ElementUsage)
				{
				case InputElementUsage::Position:
					iedesc[i].SemanticName = "SV_POSITION";
					break;
				case InputElementUsage::Normal:
					iedesc[i].SemanticName = "NORMAL";
					break;
				case InputElementUsage::TextureCoordinate:
					iedesc[i].SemanticName = "TEXCOORD";
					break;
				case InputElementUsage::Color:
					iedesc[i].SemanticName = "COLOR";
					break;
				}

				iedesc[i].SemanticIndex = e[i].UsageIndex;

				switch (e[i].ElementFormat)
				{
				case InputElementFormat::Single:
					iedesc[i].Format = DXGI_FORMAT_R32_FLOAT;
					break;
				case InputElementFormat::Vector2:
					iedesc[i].Format = DXGI_FORMAT_R32G32_FLOAT;
					break;
				case InputElementFormat::Vector3:
					iedesc[i].Format = DXGI_FORMAT_R32G32B32_FLOAT;
					break;
				case InputElementFormat::Vector4:
					iedesc[i].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
					break;
				case InputElementFormat::Color:
					iedesc[i].Format = DXGI_FORMAT_R8G8B8A8_UNORM;
					break;
				}

				iedesc[i].InputSlot = 0;
				iedesc[i].AlignedByteOffset = e[i].Offset;
				iedesc[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
				iedesc[i].InstanceDataStepRate = 0;
			}

			ID3D11InputLayout* layout;
			HRESULT r = m_d3d11State.Device->CreateInputLayout(iedesc, desc->NumElements, desc->VertexShaderBytecode.Bytecode, desc->VertexShaderBytecode.BytecodeLength, &layout);
			if (FAILED(r))
			{
				NXNA_SET_ERROR_DETAILS(r, "CreateInputLayout() failed");
				return NxnaResult::UnknownError;
			}

			result->Direct3D11.Layout = layout;
			result->Direct3D11.VertexShader = desc->VertexShader->Direct3D11.Ptr;
			result->Direct3D11.PixelShader = desc->PixelShader->Direct3D11.Ptr;

			if (result->Direct3D11.VertexShader != nullptr) ((ID3D11VertexShader*)result->Direct3D11.VertexShader)->AddRef();
			if (result->Direct3D11.PixelShader != nullptr) ((ID3D11PixelShader*)result->Direct3D11.PixelShader)->AddRef();
		}
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			unsigned int pipeline;
			glGenProgramPipelines(1, &pipeline);
			if (desc->VertexShader != nullptr)
				glUseProgramStages(pipeline, GL_VERTEX_SHADER_BIT, desc->VertexShader->OpenGL.Handle);
			if (desc->PixelShader != nullptr)
				glUseProgramStages(pipeline, GL_FRAGMENT_SHADER_BIT, desc->PixelShader->OpenGL.Handle);

			result->OpenGL.Pipeline = pipeline;
		}
		}

		return NxnaResult::Success;
	}

	void GraphicsDevice::SetShaderPipeline(ShaderPipeline* pipeline)
	{
		if (pipeline != nullptr)
		{
			switch (GetType())
			{
#ifdef NXNA_ENABLE_DIRECT3D11
			case GraphicsDeviceType::Direct3D11:
			{
				if (pipeline != nullptr)
				{
					m_d3d11State.Context->IASetInputLayout((ID3D11InputLayout*)pipeline->Direct3D11.Layout);
					m_d3d11State.Context->VSSetShader((ID3D11VertexShader*)pipeline->Direct3D11.VertexShader, nullptr, 0);
					m_d3d11State.Context->PSSetShader((ID3D11PixelShader*)pipeline->Direct3D11.PixelShader, nullptr, 0);
				}
				else
				{
					m_d3d11State.Context->IASetInputLayout(nullptr);
					m_d3d11State.Context->VSSetShader(nullptr, nullptr, 0);
					m_d3d11State.Context->PSSetShader(nullptr, nullptr, 0);
				}
			}
				break;
#endif
			case GraphicsDeviceType::OpenGl41:
			{
				if (pipeline != nullptr)
				{
					if (m_shaderPipeline != pipeline)
						glBindProgramPipeline(pipeline->OpenGL.Pipeline);
				}
				else
				{
					glBindProgramPipeline(0);
				}
			}
				break;
			}
		}

		m_shaderPipeline = pipeline;
	}

	void GraphicsDevice::DestroyShaderPipeline(ShaderPipeline* pipeline)
	{
		NXNA_VALIDATION_ASSERT(pipeline != nullptr, "pipeline cannot be null");
		NXNA_VALIDATION_ASSERT(m_shaderPipeline != pipeline, "pipeline cannot be the current ShaderPipeline");

		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			if (pipeline->Direct3D11.VertexShader != nullptr) { ((ID3D11VertexShader*)pipeline->Direct3D11.VertexShader)->Release(); pipeline->Direct3D11.VertexShader = nullptr; }
			if (pipeline->Direct3D11.PixelShader != nullptr) { ((ID3D11PixelShader*)pipeline->Direct3D11.PixelShader)->Release(); pipeline->Direct3D11.PixelShader = nullptr; }
			if (pipeline->Direct3D11.Layout != nullptr) { ((ID3D11InputLayout*)pipeline->Direct3D11.Layout)->Release(); pipeline->Direct3D11.Layout = nullptr; }

			break;
		}
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			glDeleteProgramPipelines(1, &pipeline->OpenGL.Pipeline);
			pipeline->OpenGL.Pipeline = 0;
		}
			break;
		}
	}

#define CONVERT_D3D11_BLEND(blend, dest) \
	switch(blend) { \
		case Blend::Zero: dest = D3D11_BLEND_ZERO; break;\
		case Blend::One: dest = D3D11_BLEND_ONE; break; \
		case Blend::SourceAlpha: dest = D3D11_BLEND_SRC_ALPHA; break; \
		case Blend::DestinationAlpha: dest = D3D11_BLEND_DEST_ALPHA; break; \
		case Blend::InverseSourceAlpha: dest = D3D11_BLEND_INV_SRC_ALPHA; break; \
		case Blend::InverseDestinationAlpha: dest = D3D11_BLEND_INV_DEST_ALPHA; break; \
		default: dest = D3D11_BLEND_ZERO; break; \
		};

#define CONVERT_D3D11_BLEND_FUNC(blend, dest) \
	switch(blend) { \
		case BlendFunction::Add: dest = D3D11_BLEND_OP_ADD; break; \
		case BlendFunction::Subtract: dest = D3D11_BLEND_OP_SUBTRACT; break; \
		case BlendFunction::ReverseSubtract: dest = D3D11_BLEND_OP_REV_SUBTRACT; break; \
		case BlendFunction::Min: dest = D3D11_BLEND_OP_MIN; break; \
		case BlendFunction::Max: dest = D3D11_BLEND_OP_MAX; break; \
		default: dest = D3D11_BLEND_OP_MAX; break; \
		};

#define CONVERT_GL_BLEND(blend, dest) \
	switch(blend) { \
		case Blend::Zero: dest = GL_ZERO; break;\
		case Blend::One: dest = GL_ONE; break; \
		case Blend::SourceAlpha: dest = GL_SRC_ALPHA; break; \
		case Blend::DestinationAlpha: dest = GL_DST_ALPHA; break; \
		case Blend::InverseSourceAlpha: dest = GL_ONE_MINUS_SRC_ALPHA; break; \
		case Blend::InverseDestinationAlpha: dest = GL_ONE_MINUS_DST_ALPHA; break; \
		default: dest = GL_ZERO; break; \
		};

#define CONVERT_GL_BLEND_FUNC(blend, dest) \
	switch(blend) { \
		case BlendFunction::Add: dest = GL_FUNC_ADD; break; \
		case BlendFunction::Subtract: dest = GL_FUNC_SUBTRACT; break; \
		case BlendFunction::ReverseSubtract: dest = GL_FUNC_REVERSE_SUBTRACT; break; \
		case BlendFunction::Min: dest = GL_MIN; break; \
		case BlendFunction::Max: dest = GL_MAX; break; \
		default: dest = GL_MAX; break; \
		};

	NxnaResult GraphicsDevice::CreateBlendState(const BlendStateDesc* desc, BlendState* result)
	{
		NXNA_VALIDATION_ASSERT(desc != nullptr, "desc cannot be null");
		NXNA_VALIDATION_ASSERT(result != nullptr, "result cannot be null");

		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			D3D11_BLEND_DESC blendState;
			ZeroMemory(&blendState, sizeof(D3D11_BLEND_DESC));
			blendState.RenderTarget[0].BlendEnable = desc->RenderTarget[0].BlendingEnabled;
			blendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			CONVERT_D3D11_BLEND(desc->RenderTarget[0].ColorSourceBlend, blendState.RenderTarget[0].SrcBlend);
			CONVERT_D3D11_BLEND(desc->RenderTarget[0].ColorDestinationBlend, blendState.RenderTarget[0].DestBlend);
			CONVERT_D3D11_BLEND(desc->RenderTarget[0].AlphaSourceBlend, blendState.RenderTarget[0].SrcBlendAlpha);
			CONVERT_D3D11_BLEND(desc->RenderTarget[0].AlphaDestinationBlend, blendState.RenderTarget[0].DestBlendAlpha);

			CONVERT_D3D11_BLEND_FUNC(desc->RenderTarget[0].ColorBlendFunction, blendState.RenderTarget[0].BlendOp);
			CONVERT_D3D11_BLEND_FUNC(desc->RenderTarget[0].AlphaBlendFunction, blendState.RenderTarget[0].BlendOpAlpha);

			if (desc->IndependentBlendEnabled)
			{
				for (int i = 1; i < 8; i++)
				{
					blendState.RenderTarget[i].BlendEnable = desc->RenderTarget[i].BlendingEnabled;
					blendState.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

					CONVERT_D3D11_BLEND(desc->RenderTarget[i].ColorSourceBlend, blendState.RenderTarget[i].SrcBlend);
					CONVERT_D3D11_BLEND(desc->RenderTarget[i].ColorDestinationBlend, blendState.RenderTarget[i].DestBlend);
					CONVERT_D3D11_BLEND(desc->RenderTarget[i].AlphaSourceBlend, blendState.RenderTarget[i].SrcBlendAlpha);
					CONVERT_D3D11_BLEND(desc->RenderTarget[i].AlphaDestinationBlend, blendState.RenderTarget[i].DestBlendAlpha);

					CONVERT_D3D11_BLEND_FUNC(desc->RenderTarget[i].ColorBlendFunction, blendState.RenderTarget[i].BlendOp);
					CONVERT_D3D11_BLEND_FUNC(desc->RenderTarget[i].AlphaBlendFunction, blendState.RenderTarget[i].BlendOpAlpha);
				}
			}

			HRESULT r = m_d3d11State.Device->CreateBlendState(&blendState, &result->Direct3D11.State);
			if (FAILED(r))
			{
				NXNA_SET_ERROR_DETAILS(r, "CreateBlendState() failed");
				return NxnaResult::UnknownError;
			}

			break;
		}
#endif
		case GraphicsDeviceType::OpenGLES3:
		{
			if (desc->IndependentBlendEnabled)
			{
				NXNA_SET_ERROR_DETAILS(0, "Independent blending is not supported");
				return NxnaResult::UnknownError; // not supported on ES3
			}
		}
			// no break, just drop through
		case GraphicsDeviceType::OpenGl41:
		{
			result->OpenGL.Desc = *desc;
		}
			break;
		}

		return NxnaResult::Success;
	}

	void GraphicsDevice::SetBlendState(BlendState* state)
	{
		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			if (m_blendState == nullptr || m_blendState->Direct3D11.State != state->Direct3D11.State)
				m_d3d11State.Context->OMSetBlendState((ID3D11BlendState*)state->Direct3D11.State, nullptr, 0xffffffff);
		}
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
#define SET_GL_BLENDING_FOR_RT(rtIndex, state) \
									{ \
								if (m_blendState == nullptr || m_blendState->OpenGL.Desc.RenderTarget[rtIndex].BlendingEnabled != state->OpenGL.Desc.RenderTarget[rtIndex].BlendingEnabled) \
																{ \
									if (state->OpenGL.Desc.RenderTarget[rtIndex].BlendingEnabled) \
										glEnablei(GL_BLEND, rtIndex); \
																		else \
										glDisablei(GL_BLEND, rtIndex); \
																} \
								\
								GLenum colorSrc, colorDest, alphaSrc, alphaDest; \
								CONVERT_GL_BLEND(state->OpenGL.Desc.RenderTarget[rtIndex].ColorSourceBlend, colorSrc); \
								CONVERT_GL_BLEND(state->OpenGL.Desc.RenderTarget[rtIndex].ColorDestinationBlend, colorDest); \
								CONVERT_GL_BLEND(state->OpenGL.Desc.RenderTarget[rtIndex].AlphaSourceBlend, alphaSrc); \
								CONVERT_GL_BLEND(state->OpenGL.Desc.RenderTarget[rtIndex].AlphaDestinationBlend, alphaDest); \
								glBlendFuncSeparatei(rtIndex, colorSrc, colorDest, alphaSrc, alphaDest); \
								\
								GLenum colorFunc, alphaFunc; \
								CONVERT_GL_BLEND_FUNC(state->OpenGL.Desc.RenderTarget[rtIndex].ColorBlendFunction, colorFunc); \
								CONVERT_GL_BLEND_FUNC(state->OpenGL.Desc.RenderTarget[rtIndex].AlphaBlendFunction, alphaFunc); \
								glBlendEquationSeparatei(rtIndex, colorFunc, alphaFunc); \
									}

			if (m_blendState == nullptr || m_blendState->OpenGL.Desc.IndependentBlendEnabled != state->OpenGL.Desc.IndependentBlendEnabled)
			{
				if (state->OpenGL.Desc.IndependentBlendEnabled == true)
					goto allBuffers;
				else
					goto allSeparate;
			}
			else if (state->OpenGL.Desc.IndependentBlendEnabled == false)
			{
				if (memcmp(&m_blendState->OpenGL.Desc.RenderTarget[0], &state->OpenGL.Desc.RenderTarget[0], sizeof(RenderTargetBlendStateDesc)) != 0)
					goto allBuffers;
				else
				{
					// no changes necesary
				}
			}
			else if (state->OpenGL.Desc.IndependentBlendEnabled == true)
			{
				for (int i = 0; i < 8; i++)
				{
					if (memcmp(&m_blendState->OpenGL.Desc.RenderTarget[i], &state->OpenGL.Desc.RenderTarget[i], sizeof(RenderTargetBlendStateDesc)))
						SET_GL_BLENDING_FOR_RT(i, state);
				}
			}


		allBuffers:
			{
				if (m_blendState == nullptr || m_blendState->OpenGL.Desc.RenderTarget[0].BlendingEnabled != state->OpenGL.Desc.RenderTarget[0].BlendingEnabled)
				{
					if (state->OpenGL.Desc.RenderTarget[0].BlendingEnabled)
						glEnable(GL_BLEND);
					else
						glDisable(GL_BLEND);
				}

				GLenum colorSrc, colorDest, alphaSrc, alphaDest;
				CONVERT_GL_BLEND(state->OpenGL.Desc.RenderTarget[0].ColorSourceBlend, colorSrc);
				CONVERT_GL_BLEND(state->OpenGL.Desc.RenderTarget[0].ColorDestinationBlend, colorDest);
				CONVERT_GL_BLEND(state->OpenGL.Desc.RenderTarget[0].AlphaSourceBlend, alphaSrc);
				CONVERT_GL_BLEND(state->OpenGL.Desc.RenderTarget[0].AlphaDestinationBlend, alphaDest);
				glBlendFuncSeparate(colorSrc, colorDest, alphaSrc, alphaDest);

				GLenum colorFunc, alphaFunc;
				CONVERT_GL_BLEND_FUNC(state->OpenGL.Desc.RenderTarget[0].ColorBlendFunction, colorFunc);
				CONVERT_GL_BLEND_FUNC(state->OpenGL.Desc.RenderTarget[0].AlphaBlendFunction, alphaFunc);
				glBlendEquationSeparate(colorFunc, alphaFunc);

				goto end;
			}

		allSeparate:
			{
				for (int i = 0; i < 8; i++)
				{
					SET_GL_BLENDING_FOR_RT(i, state);
				}

				goto end;
			}

		end:
			;
		}
			break;
		}

		m_blendState = state;
	}

	void GraphicsDevice::DestroyBlendState(BlendState* state)
	{
		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			state->Direct3D11.State->Release();
			state->Direct3D11.State = nullptr;
		}
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			// nothing
		}
			break;
		}
	}

	NxnaResult GraphicsDevice::CreateRasterizerState(const RasterizerStateDesc* desc, RasterizerState* result)
	{
		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			D3D11_RASTERIZER_DESC rasterDesc;

			switch(desc->CullingMode)
			{
			case CullMode::CullFrontFaces: rasterDesc.CullMode = D3D11_CULL_FRONT; break;
			case CullMode::CullBackFaces: rasterDesc.CullMode = D3D11_CULL_BACK; break;
			default: rasterDesc.CullMode = D3D11_CULL_NONE; break;
			}

			switch(desc->FillingMode)
			{
			case FillMode::Wireframe: rasterDesc.FillMode = D3D11_FILL_WIREFRAME; break;
			default: rasterDesc.FillMode = D3D11_FILL_SOLID; break;
			}

			rasterDesc.AntialiasedLineEnable = false;
			rasterDesc.DepthBias = 0;
			rasterDesc.DepthBiasClamp = 0.0f;
			rasterDesc.DepthClipEnable = true;
			rasterDesc.FrontCounterClockwise = desc->FrontCounterClockwise;
			rasterDesc.MultisampleEnable = false;
			rasterDesc.ScissorEnable = desc->ScissorTestEnabled;
			rasterDesc.SlopeScaledDepthBias = 0.0f;

			// Create the rasterizer state from the description we just filled out.
			HRESULT r = m_d3d11State.Device->CreateRasterizerState(&rasterDesc, &result->Direct3D11.State);
			if (FAILED(r))
			{
				NXNA_SET_ERROR_DETAILS(r, "CreateRasterizerState() failed");
				return NxnaResult::UnknownError;
			}
		}
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			result->OpenGL.Desc = *desc;
		}
			break;
		}

		return NxnaResult::Success;
	}

	void GraphicsDevice::SetRasterizerState(RasterizerState* state)
	{
		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			if (m_rasterizerState == nullptr || state->Direct3D11.State != m_rasterizerState->Direct3D11.State)
				m_d3d11State.Context->RSSetState(state->Direct3D11.State);
		}
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			if (m_rasterizerState == nullptr ||
				state->OpenGL.Desc.CullingMode != state->OpenGL.Desc.CullingMode ||
				state->OpenGL.Desc.FrontCounterClockwise != state->OpenGL.Desc.FrontCounterClockwise)
			{
				switch (state->OpenGL.Desc.CullingMode)
				{
				case CullMode::None:
					glDisable(GL_CULL_FACE);
					break;
				case CullMode::CullBackFaces:
					glEnable(GL_CULL_FACE);
					glFrontFace(state->OpenGL.Desc.FrontCounterClockwise ? GL_CCW : GL_CW);
					break;
				case CullMode::CullFrontFaces:
					glEnable(GL_CULL_FACE);
					glFrontFace(state->OpenGL.Desc.FrontCounterClockwise ? GL_CW : GL_CCW);
					break;
				}
			}

			if (m_rasterizerState == nullptr || state->OpenGL.Desc.ScissorTestEnabled != m_rasterizerState->OpenGL.Desc.ScissorTestEnabled)
			{
				if (state->OpenGL.Desc.ScissorTestEnabled)
					glEnable(GL_SCISSOR_TEST);
				else
					glDisable(GL_SCISSOR_TEST);
			}

			if (m_rasterizerState == nullptr || state->OpenGL.Desc.FillingMode != m_rasterizerState->OpenGL.Desc.FillingMode)
			{
				if (state->OpenGL.Desc.FillingMode == FillMode::Solid)
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				else
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
		}
			break;
		}

		m_rasterizerState = state;
	}

	void GraphicsDevice::DestroyRasterizerState(RasterizerState* state)
	{
		if (m_rasterizerState == state)
			m_rasterizerState = nullptr;

		switch(GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			state->Direct3D11.State->Release();
			state->Direct3D11.State = nullptr;
		}
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			// nothing
		}
			break;
		}
	}

	
	NxnaResult GraphicsDevice::CreateIndexBuffer(const IndexBufferDesc* desc, IndexBuffer* result)
	{
		result->ElementSize = desc->ElementSize;

#ifndef NXNA_DISABLE_VALIDATION
		result->BufferUsage = desc->BufferUsage;
		result->ByteLength = (int)desc->ElementSize * desc->NumElements;
		NXNA_VALIDATION_ASSERT(desc->InitialDataByteCount <= result->ByteLength, "Inital data is too long");
#endif

		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			D3D11_BUFFER_DESC bdesc;
			ZeroMemory(&bdesc, sizeof(D3D11_BUFFER_DESC));
			bdesc.ByteWidth = (int)desc->ElementSize * desc->NumElements;
			bdesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			switch(desc->BufferUsage)
			{
			case Usage::Immutable:
				bdesc.Usage = D3D11_USAGE_IMMUTABLE;
				break;
			case Usage::Dynamic:
				bdesc.Usage = D3D11_USAGE_DYNAMIC;
				bdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				break;
			case Usage::Default:
			default:
				bdesc.Usage = D3D11_USAGE_DEFAULT;
				break;
			}

			HRESULT r = m_d3d11State.Device->CreateBuffer(&bdesc, nullptr, &result->Direct3D11.Buffer);
			if (FAILED(r))
			{
				NXNA_SET_ERROR_DETAILS(r, "CreateBuffer() failed");
				return NxnaResult::UnknownError;
			}

			if (desc->InitialData != nullptr)
			{
				D3D11_BOX box;
				box.top = 0;
				box.front = 0;
				box.back = 1;
				box.bottom = 1;
				box.left = 0;
				box.right = desc->InitialDataByteCount;

				m_d3d11State.Context->UpdateSubresource(result->Direct3D11.Buffer, 0, &box, desc->InitialData, 1, 0);
			}

			return NxnaResult::Success;
		}
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			result->OpenGL.ByteLength = (int)desc->ElementSize * desc->NumElements;

			glGenBuffers(1, &result->OpenGL.Buffer);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, result->OpenGL.Buffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, desc->InitialDataByteCount, desc->InitialData, GL_STATIC_DRAW);

			return NxnaResult::Success;
		}
			break;
		default:
			NXNA_SET_ERROR_DETAILS(0, "Unsupported API");
			return NxnaResult::UnknownError;
		}
	}

	void GraphicsDevice::DestroyIndexBuffer(IndexBuffer buffer)
	{
		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
			// TODO
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			glDeleteBuffers(1, &buffer.OpenGL.Buffer);
		}
			break;
		default:
			NXNA_SET_ERROR_DETAILS(0, "Unsupported graphics device type");
		}
	}

	void GraphicsDevice::UpdateIndexBuffer(IndexBuffer buffer, unsigned int startOffset, void* data, unsigned int dataLengthInBytes)
	{
		NXNA_VALIDATION_ASSERT(startOffset + dataLengthInBytes <= buffer.ByteLength, "Data is too long");
		NXNA_VALIDATION_ASSERT(buffer.BufferUsage != Usage::Dynamic, "Index buffer cannot be Dynamic");

		// TODO: only allow index buffers created with Usage::Dynamic to be updated

		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			D3D11_BOX box;
			box.top = 0;
			box.front = 0;
			box.back = 1;
			box.bottom = 1;
			box.left = startOffset;
			box.right = startOffset + dataLengthInBytes;

			m_d3d11State.Context->UpdateSubresource(buffer.Direct3D11.Buffer, 0, &box, data, 1, 0);
		}
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.OpenGL.Buffer);
			if (startOffset == 0)
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, dataLengthInBytes, data, GL_STATIC_DRAW);
			else
				glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, startOffset, dataLengthInBytes, data);
		}
			break;
		default:
			NXNA_SET_ERROR_DETAILS(0, "Unsupported graphics device type");
		}
	}

	void GraphicsDevice::SetIndices(IndexBuffer indices)
	{
		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			DXGI_FORMAT format;
			switch(indices.ElementSize)
			{
			case IndexElementSize::SixteenBits:
				format = DXGI_FORMAT_R16_UINT;
				break;
			default:
				format = DXGI_FORMAT_R32_UINT;
				break;
			}

			m_d3d11State.Context->IASetIndexBuffer(indices.Direct3D11.Buffer, format, 0);
		}
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices.OpenGL.Buffer);
		}
			break;
		default:
			NXNA_SET_ERROR_DETAILS(0, "Unsupported graphics device type");
		}

		m_indices = indices;
	}

	NxnaResult GraphicsDevice::CreateVertexBuffer(const VertexBufferDesc* desc, VertexBuffer* result)
	{
#ifndef NXNA_DISABLE_VALIDATION
		result->BufferUsage = desc->BufferUsage;
		result->ByteLength = desc->StrideBytes * desc->NumVertices;
		NXNA_VALIDATION_ASSERT(desc->InitialDataByteCount <= result->ByteLength, "Inital data is too long");
#endif

		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			HRESULT r;
			D3D11_BUFFER_DESC vbdesc;
			ZeroMemory(&vbdesc, sizeof(D3D11_BUFFER_DESC));
			vbdesc.ByteWidth = desc->NumVertices * desc->StrideBytes;
			vbdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			switch(desc->BufferUsage)
			{
			case Usage::Immutable:
				vbdesc.Usage = D3D11_USAGE_IMMUTABLE;
				break;
			case Usage::Dynamic:
				vbdesc.Usage = D3D11_USAGE_DYNAMIC;
				vbdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				break;
			case Usage::Default:
			default:
				vbdesc.Usage = D3D11_USAGE_DEFAULT;
				break;
			}

			if (desc->InitialData != nullptr)
			{
				D3D11_SUBRESOURCE_DATA InitData;
				InitData.pSysMem = desc->InitialData;
				InitData.SysMemPitch = 0;
				InitData.SysMemSlicePitch = 0;

				r = m_d3d11State.Device->CreateBuffer(&vbdesc, &InitData, &result->Direct3D11.Buffer);
				if (FAILED(r))
				{
					NXNA_SET_ERROR_DETAILS(r, "CreateBuffer() failed");
					return NxnaResult::UnknownError;
				}
			}
			else
			{
				r = m_d3d11State.Device->CreateBuffer(&vbdesc, nullptr, &result->Direct3D11.Buffer);
				if (FAILED(r))
				{
					NXNA_SET_ERROR_DETAILS(r, "CreateBuffer() failed");
					return NxnaResult::UnknownError;
				}
			}
		}
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			result->OpenGL.ByteLength = desc->StrideBytes * desc->NumVertices;

			glGenBuffers(1, &result->OpenGL.Buffer);
			glBindBuffer(GL_ARRAY_BUFFER, result->OpenGL.Buffer);
			glBufferData(GL_ARRAY_BUFFER, result->OpenGL.ByteLength, nullptr, GL_STATIC_DRAW);
			if (desc->InitialData != nullptr)
				glBufferSubData(GL_ARRAY_BUFFER, 0, desc->InitialDataByteCount, desc->InitialData);

			// setup the VAO
			{
				glGenVertexArrays(1, &result->OpenGL.VAO);
				glBindVertexArray(result->OpenGL.VAO);

				for (int i = 0; i < desc->NumInputElements; i++)
				{
					int sizeOfElement = 0;
					GLenum type;
					GLboolean normalize;

					auto format = desc->InputElements[i].ElementFormat;
					if (format == InputElementFormat::Color)
					{
						sizeOfElement = 4;
						type = GL_UNSIGNED_BYTE;
						normalize = GL_TRUE;
					}
					else
					{
						sizeOfElement = (int)format;
						type = GL_FLOAT;
						normalize = GL_FALSE;
					}

					glEnableVertexAttribArray(i);
					glVertexAttribPointer(i, sizeOfElement, type, normalize, desc->StrideBytes, (void*)(intptr_t)desc->InputElements[i].Offset);
				}
			}

			return NxnaResult::Success;
		}
			break;
		default:
			NXNA_SET_ERROR_DETAILS(0, "Unknown API");
			return NxnaResult::UnknownError;
		}

		return NxnaResult::Success;
	}

	void GraphicsDevice::DestroyVertexBuffer(VertexBuffer buffer)
	{
		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
			// TODO
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			glDeleteBuffers(1, &buffer.OpenGL.Buffer);
			glDeleteVertexArrays(1, &buffer.OpenGL.VAO);
		}
			break;
		default:
			NXNA_SET_ERROR_DETAILS(0, "Unsupported graphics device type");
		}
	}

	void GraphicsDevice::UpdateVertexBuffer(VertexBuffer buffer, unsigned int startOffset, void* data, unsigned int dataLengthInBytes)
	{
		NXNA_VALIDATION_ASSERT(startOffset + dataLengthInBytes <= buffer.ByteLength, "Data is too long");
		NXNA_VALIDATION_ASSERT(buffer.BufferUsage != Usage::Dynamic, "Vertex buffer cannot be Dynamic");

		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			D3D11_BOX box;
			box.top = 0;
			box.front = 0;
			box.back = 1;
			box.bottom = 1;
			box.left = startOffset;
			box.right = startOffset + dataLengthInBytes;
			m_d3d11State.Context->UpdateSubresource(buffer.Direct3D11.Buffer, 0, &box, data, 1, 0);
		}
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			glBindBuffer(GL_ARRAY_BUFFER, buffer.OpenGL.Buffer);
			if (startOffset == 0)
				glBufferData(GL_ARRAY_BUFFER, dataLengthInBytes, data, GL_STATIC_DRAW);
			else
				glBufferSubData(GL_ARRAY_BUFFER, startOffset, dataLengthInBytes, data);
		}
			break;
		default:
			NXNA_SET_ERROR_DETAILS(0, "Unsupported graphics device type");
		}
	}

	void GraphicsDevice::SetVertexBuffer(VertexBuffer vertexBuffer, unsigned int offset, unsigned int stride)
	{
		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			m_d3d11State.Context->IASetVertexBuffers(0, 1, &vertexBuffer.Direct3D11.Buffer, &stride, &offset);
		}
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			glBindVertexArray(vertexBuffer.OpenGL.VAO);
		}
			break;
		default:
			NXNA_SET_ERROR_DETAILS(0, "Unsupported graphics device type");
		}

		m_vertices = m_vertices;
	}

	NxnaResult GraphicsDevice::CreateConstantBuffer(const ConstantBufferDesc* desc, ConstantBuffer* result)
	{
#ifndef NXNA_DISABLE_VALIDATION
		result->BufferUsage = desc->BufferUsage;
		result->ByteLength = desc->ByteCount;
		NXNA_VALIDATION_ASSERT(desc->ByteCount <= result->ByteLength, "Inital data is too long");
		NXNA_VALIDATION_ASSERT(desc->ByteCount % 16 == 0, "sizeInBytes must be a multiple of 16");
#endif

		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			HRESULT r;
			D3D11_BUFFER_DESC cbdesc;
			ZeroMemory(&cbdesc, sizeof(D3D11_BUFFER_DESC));
			cbdesc.ByteWidth = desc->ByteCount;
			cbdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbdesc.MiscFlags = 0;
			cbdesc.StructureByteStride = 0;
			switch(desc->BufferUsage)
			{
			case Usage::Immutable:
				cbdesc.Usage = D3D11_USAGE_IMMUTABLE;
				break;
			case Usage::Dynamic:
				cbdesc.Usage = D3D11_USAGE_DYNAMIC;
				cbdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				break;
			case Usage::Default:
			default:
				cbdesc.Usage = D3D11_USAGE_DEFAULT;
				break;
			}

			if (desc->InitialData != nullptr)
			{
				D3D11_SUBRESOURCE_DATA InitData;
				InitData.pSysMem = desc->InitialData;
				InitData.SysMemPitch = 0;
				InitData.SysMemSlicePitch = 0;

				r = m_d3d11State.Device->CreateBuffer(&cbdesc, &InitData, &result->Direct3D11.Buffer);
				if (FAILED(r))
				{
					NXNA_SET_ERROR_DETAILS(r, "CreateBuffer() failed");
					return NxnaResult::UnknownError;
				}
			}
			else
			{
				r = m_d3d11State.Device->CreateBuffer(&cbdesc, nullptr, &result->Direct3D11.Buffer);
				if (FAILED(r))
				{
					NXNA_SET_ERROR_DETAILS(r, "CreateBuffer() failed");
					return NxnaResult::UnknownError;
				}
			}
		}
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			result->OpenGL.ByteLength = desc->ByteCount;

			glGenBuffers(1, &result->OpenGL.UniformBuffer);
			glBindBuffer(GL_UNIFORM_BUFFER, result->OpenGL.UniformBuffer);

			GLenum glUsage;
			switch (desc->BufferUsage)
			{
			case Usage::Default:
				glUsage = GL_STATIC_DRAW;
				break;
			case Usage::Dynamic:
				glUsage = GL_DYNAMIC_DRAW;
				break;
			}

			glBufferData(GL_UNIFORM_BUFFER, desc->ByteCount, desc->InitialData, glUsage);
		}
		}

		return NxnaResult::Success;
	}

	void GraphicsDevice::UpdateConstantBuffer(ConstantBuffer buffer, void* data, unsigned int dataLengthInBytes)
	{
		NXNA_VALIDATION_ASSERT(dataLengthInBytes == buffer.ByteLength, "Data length is not correct");
		NXNA_VALIDATION_ASSERT(buffer.BufferUsage != Usage::Dynamic, "Constant buffer cannot be Dynamic");

		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			m_d3d11State.Context->UpdateSubresource(buffer.Direct3D11.Buffer, 0, nullptr, data, 1, 0);
		}
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			glBindBuffer(GL_ARRAY_BUFFER, buffer.OpenGL.UniformBuffer);
			glBufferSubData(GL_ARRAY_BUFFER, 0, dataLengthInBytes, data);
		}
			break;
		default:
			NXNA_SET_ERROR_DETAILS(0, "Unsupported graphics device type");
		}
	}

	void GraphicsDevice::SetConstantBuffer(ConstantBuffer buffer, int index)
	{
		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			m_d3d11State.Context->VSSetConstantBuffers(0, 1, &buffer.Direct3D11.Buffer);
		}
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			glBindBufferRange(GL_UNIFORM_BUFFER, index, buffer.OpenGL.UniformBuffer, 0, buffer.OpenGL.ByteLength);
		}
		}
	}

	void GraphicsDevice::DestroyConstantBuffer(ConstantBuffer* buffer)
	{
		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
			// TODO
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			glDeleteBuffers(1, &buffer->OpenGL.UniformBuffer);
		}
		}
	}

#define D3D11_MAP_BUFFER \
	D3D11_MAP access; \
	switch (type) \
		{ \
	case MapType::Write: access = D3D11_MAP_WRITE; break; \
	case MapType::WriteDiscard: access = D3D11_MAP_WRITE_DISCARD; break; \
	case MapType::WriteNoOverwrite: access = D3D11_MAP_WRITE_NO_OVERWRITE; break; \
		} \
	\
	D3D11_MAPPED_SUBRESOURCE map; \
	r = m_d3d11State.Context->Map(buffer.Direct3D11.Buffer, 0, access, 0, &map); \
	if (FAILED(r)) \
	{ \
		NXNA_SET_ERROR_DETAILS(r, "Map() failed"); \
 		return nullptr; \
	} \
	\
	return map.pData;

#define OGL_CONVERT_MAP_ACCESS(s, d) \
	switch (s) \
	{ \
	case MapType::Write: d = GL_MAP_WRITE_BIT; break; \
	case MapType::WriteDiscard: d = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT; break; \
	case MapType::WriteNoOverwrite: d = GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT; break; \
	default: return nullptr; \
	}

	void* GraphicsDevice::MapBuffer(IndexBuffer buffer, MapType type)
	{
#ifndef NXNA_DISABLE_VALIDATION
		if (type == MapType::WriteDiscard || type == MapType::WriteNoOverwrite)
			if (buffer.BufferUsage != Usage::Dynamic)
				NXNA_VALIDATION_ASSERT(false, "Map type must be used with a buffer created as Dynamic");
#endif

		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			HRESULT r;
			D3D11_MAP_BUFFER
		}
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			GLbitfield access;
			OGL_CONVERT_MAP_ACCESS(type, access);

			void* ptr;
			if (glMapNamedBuffer != nullptr)
			{
				ptr = glMapNamedBufferRange(buffer.OpenGL.Buffer, 0, buffer.OpenGL.ByteLength, access);
			}
			else
			{
				// remember the original buffer
				GLint currentBuffer;
				glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &currentBuffer);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.OpenGL.Buffer);
				ptr = glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, buffer.OpenGL.ByteLength, access);

				// restore the old buffer
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, currentBuffer);
			}

			return ptr;
		}
			break;
		}

		return nullptr;
	}

	void* GraphicsDevice::MapBuffer(VertexBuffer buffer, MapType type)
	{
#ifndef NXNA_DISABLE_VALIDATION
		if (type == MapType::WriteDiscard || type == MapType::WriteNoOverwrite)
			if (buffer.BufferUsage != Usage::Dynamic)
				NXNA_VALIDATION_ASSERT(false, "Map type must be used with a buffer created as Dynamic");
#endif

		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			HRESULT r;
			D3D11_MAP_BUFFER
		}
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			GLbitfield access;
			OGL_CONVERT_MAP_ACCESS(type, access);

			void* ptr;
			if (glMapNamedBuffer != nullptr)
			{
				ptr = glMapNamedBufferRange(buffer.OpenGL.Buffer, 0, buffer.OpenGL.ByteLength, access);
			}
			else
			{
				// remember the original buffer
				GLint currentBuffer;
				glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &currentBuffer);

				glBindBuffer(GL_ARRAY_BUFFER, buffer.OpenGL.Buffer);
				ptr = glMapBufferRange(GL_ARRAY_BUFFER, 0, buffer.OpenGL.ByteLength, access);

				// restore the old buffer
				glBindBuffer(GL_ARRAY_BUFFER, currentBuffer);
			}

			return ptr;
		}
			break;
		}

		return nullptr;
	}

	void* GraphicsDevice::MapBuffer(ConstantBuffer buffer, MapType type)
	{
#ifndef NXNA_DISABLE_VALIDATION
		if (type == MapType::WriteDiscard || type == MapType::WriteNoOverwrite)
			if (buffer.BufferUsage != Usage::Dynamic)
				NXNA_VALIDATION_ASSERT(false, "Map type must be used with a buffer created as Dynamic");

		// TODO: Direct3D may or may not support MapType::WriteNoOverwrite with constant buffers, depending on the runtime version, so check for that
#endif

		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			HRESULT r;
			D3D11_MAP_BUFFER
		}
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			GLbitfield access;
			OGL_CONVERT_MAP_ACCESS(type, access);

			void* ptr;
			if (glMapNamedBuffer != nullptr)
			{
				ptr = glMapNamedBufferRange(buffer.OpenGL.UniformBuffer, 0, buffer.OpenGL.ByteLength, access);
			}
			else
			{
				// remember the original buffer
				GLint currentBuffer;
				glGetIntegerv(GL_UNIFORM_BUFFER_BINDING, &currentBuffer);

				glBindBuffer(GL_UNIFORM_BUFFER, buffer.OpenGL.UniformBuffer);
				ptr = glMapBufferRange(GL_UNIFORM_BUFFER, 0, buffer.OpenGL.ByteLength, access);

				// restore the old buffer
				glBindBuffer(GL_UNIFORM_BUFFER, currentBuffer);
			}

			return ptr;
		}
			break;
		}

		return nullptr;
	}

	void GraphicsDevice::UnmapBuffer(IndexBuffer buffer)
	{
		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			m_d3d11State.Context->Unmap(buffer.Direct3D11.Buffer, 0);
		}
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			if (glUnmapNamedBuffer != nullptr)
			{
				glUnmapNamedBuffer(buffer.OpenGL.Buffer);
			}
			else
			{
				// remember the original buffer
				GLint currentBuffer;
				glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &currentBuffer);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.OpenGL.Buffer);
				glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

				// restore the old buffer
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, currentBuffer);
			}
		}
		}
	}

	void GraphicsDevice::UnmapBuffer(VertexBuffer buffer)
	{
		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			m_d3d11State.Context->Unmap(buffer.Direct3D11.Buffer, 0);
		}
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			if (glUnmapNamedBuffer != nullptr)
			{
				glUnmapNamedBuffer(buffer.OpenGL.Buffer);
			}
			else
			{
				// remember the original buffer
				GLint currentBuffer;
				glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &currentBuffer);

				glBindBuffer(GL_ARRAY_BUFFER, buffer.OpenGL.Buffer);
				glUnmapBuffer(GL_ARRAY_BUFFER);

				// restore the old buffer
				glBindBuffer(GL_ARRAY_BUFFER, currentBuffer);
			}
		}
		}
	}

	void GraphicsDevice::UnmapBuffer(ConstantBuffer buffer)
	{
		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			m_d3d11State.Context->Unmap(buffer.Direct3D11.Buffer, 0);
		}
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			if (glUnmapNamedBuffer != nullptr)
			{
				glUnmapNamedBuffer(buffer.OpenGL.UniformBuffer);
			}
			else
			{
				// remember the original buffer
				GLint currentBuffer;
				glGetIntegerv(GL_UNIFORM_BUFFER_BINDING, &currentBuffer);

				glBindBuffer(GL_UNIFORM_BUFFER, buffer.OpenGL.UniformBuffer);
				glUnmapBuffer(GL_UNIFORM_BUFFER);

				// restore the old buffer
				glBindBuffer(GL_UNIFORM_BUFFER, currentBuffer);
			}
		}
		}
	}

	void GraphicsDevice::DrawIndexedPrimitives(PrimitiveType primitiveType, int baseVertex, int minVertexIndex, int numVertices, int startIndex, int primitiveCount)
	{
		applyDirtyStates();

		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			int indexCount;
			if (primitiveType == PrimitiveType::TriangleList)
			{
				indexCount = primitiveCount * 3;
				m_d3d11State.Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			}
			else if (primitiveType == PrimitiveType::TriangleStrip)
			{
				indexCount = primitiveCount * 3; // FIXME
				m_d3d11State.Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
			}

			m_d3d11State.Context->DrawIndexed(indexCount, startIndex, baseVertex);
		}
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			int size;
			switch (m_indices.ElementSize)
			{
			case IndexElementSize::SixteenBits:
				size = GL_UNSIGNED_SHORT;
				break;
			default:
				size = GL_UNSIGNED_INT;
				break;
			}

			GLenum glPrimitiveType;
			if (primitiveType == PrimitiveType::TriangleStrip)
				glPrimitiveType = GL_TRIANGLE_STRIP;
			else
				glPrimitiveType = GL_TRIANGLES;

			glDrawElementsBaseVertex(glPrimitiveType, primitiveCount * 3, size, (void*)(intptr_t)(startIndex * (int)m_indices.ElementSize), baseVertex);
		}
		}
	}

	void GraphicsDevice::Clear(float r, float g, float b, float a)
	{
		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			float c[4] = { r, g, b, a };
			m_d3d11State.Context->ClearRenderTargetView(m_d3d11State.RenderTargetView, c);
		}
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			float c[4] = { r, g, b, a };
			glClearBufferfv(GL_COLOR, 0, c);

			glClearBufferfi(GL_DEPTH_STENCIL, 0, 0, 0);
		}
		break;
		}
	}

	void GraphicsDevice::Present()
	{
		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			m_d3d11State.SwapChain->Present(0, 0);
		}
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			glFlush();
		}
			break;
		}
	}

	void GraphicsDevice::applyDirtyStates()
	{
		
	}
}
}
