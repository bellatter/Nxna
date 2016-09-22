﻿#include "GraphicsDevice.h"
#include "OpenGL.h"
#include "PipelineState.h"
#include <cstdio>
#include <cstring>
#include <cmath>

namespace Nxna
{
namespace Graphics
{
	static const int g_numDeviceTypes = 4;
	GraphicsDeviceMessageCallback g_callbacks[g_numDeviceTypes];

#ifdef _WIN32
#define NXNA_SET_ERROR_DETAILS(api, desc) { m_errorDetails.Filename = __FILE__; m_errorDetails.LineNumber = __LINE__; m_errorDetails.APIErrorCode = api; \
	strncpy_s(m_errorDetails.ErrorDescription, desc, 255); m_errorDetails.ErrorDescription[255] = 0; }
#else
#define NXNA_SET_ERROR_DETAILS(api, desc) { m_errorDetails.Filename = __FILE__; m_errorDetails.LineNumber = __LINE__; m_errorDetails.APIErrorCode = api; \
	strncpy(m_errorDetails.ErrorDescription, desc, 255); m_errorDetails.ErrorDescription[255] = 0; }
#endif

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

	NxnaResult GraphicsDevice::CreateGraphicsDevice(const GraphicsDeviceDesc* params, GraphicsDevice* result)
	{
		result->m_type = params->Type;
		result->m_screenWidth = params->ScreenWidth;
		result->m_screenHeight = params->ScreenHeight;

		result->m_shaderPipeline = nullptr;

		switch (params->Type)
		{
		case GraphicsDeviceType::OpenGl41:
		{
			OpenGL::LoadGLExtensions(4, 1);

			int major = 0, minor = 0;
			glGetIntegerv(GL_MAJOR_VERSION, &major);
			glGetIntegerv(GL_MINOR_VERSION, &minor);

			if (major < 4 || (major == 4 && minor < 1))
			{
				if (GLEW_ARB_separate_shader_objects == false)
				{
					// we require a 4.1+ capable context
					return NxnaResult::NotSupported;
				}
			}

			// load the capabilities
			glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, (GLint*)&result->m_caps.MaxSamplerCount);

			// set the states to the defaults
			result->SetDepthStencilState(nullptr);
			result->SetBlendState(nullptr);
			result->SetRasterizerState(nullptr);

			// create a default sampler state
			{
				glGenSamplers(1, &result->m_oglState.DefaultSamplerState);
				glSamplerParameteri(result->m_oglState.DefaultSamplerState, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glSamplerParameteri(result->m_oglState.DefaultSamplerState, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glSamplerParameteri(result->m_oglState.DefaultSamplerState, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glSamplerParameteri(result->m_oglState.DefaultSamplerState, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glSamplerParameteri(result->m_oglState.DefaultSamplerState, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				float color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
				glSamplerParameterfv(result->m_oglState.DefaultSamplerState, GL_TEXTURE_BORDER_COLOR, color);

				result->SetSamplerStates(0, result->m_caps.MaxSamplerCount, nullptr);
			}
		}
			break;
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			if (params->Direct3D11.Device == nullptr ||
				params->Direct3D11.DeviceContext == nullptr ||
				params->Direct3D11.RenderTargetView == nullptr ||
				params->Direct3D11.DepthStencilView == nullptr ||
				params->Direct3D11.SwapChain == nullptr)
			{
				return NxnaResult::InvalidArgument;
			}

			// load the capabilities
			result->m_caps.MaxSamplerCount = D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT; 

			result->m_d3d11State.Device = params->Direct3D11.Device;
			result->m_d3d11State.Context = params->Direct3D11.DeviceContext;
			result->m_d3d11State.RenderTargetView = params->Direct3D11.RenderTargetView;
			result->m_d3d11State.DepthStencilView = params->Direct3D11.DepthStencilView;
			result->m_d3d11State.SwapChain = params->Direct3D11.SwapChain;
		}
			break;
#endif
		default:
			return NxnaResult::NotSupported;
		}

		return NxnaResult::Success;
	}

	void GraphicsDevice::DestroyGraphicsDevice(GraphicsDevice* device)
	{
		// nothing... for now
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

	void GraphicsDevice::SetViewport(float x, float y, float width, float height, float minDepth, float maxDepth)
	{
		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			D3D11_VIEWPORT vp;
			vp.TopLeftX = x;
			vp.TopLeftY = y;
			vp.Width = width;
			vp.Height = height;
			vp.MinDepth = minDepth;
			vp.MaxDepth = maxDepth;

			m_d3d11State.Context->RSSetViewports(1, &vp);
		}
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			// TODO: OpenGL 4.1 (the API at least, not necessarily the hardware) supports fractional viewport sizes,
			// so use that if available. For now just truncate the fraction part.

			// OpenGL stores the bottom-left corner, but XNA
			// stores the upper-left corner, so we have to convert.
			int y2 = m_screenHeight - (int)(height + y);

			glViewport((int)x, y2, (int)width, (int)height);
			glDepthRange(minDepth, maxDepth);
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
			vp.TopLeftX = viewport.X;
			vp.TopLeftY = viewport.Y;
			vp.Width = viewport.Width;
			vp.Height = viewport.Height;
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
			int y2 = m_screenHeight - (int)(viewport.Height + viewport.Y);

			glViewport((int)viewport.X, y2, (int)viewport.Width, (int)viewport.Height);
		}
		}
	}

	Viewport GraphicsDevice::GetViewport()
	{
		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			UINT numViewports = 1;
			D3D11_VIEWPORT vp;
			m_d3d11State.Context->RSGetViewports(&numViewports, &vp);

			return Viewport(vp.TopLeftX, vp.TopLeftY, vp.Width, vp.Height);
		}
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			GLint viewport[4];
			glGetIntegerv(GL_VIEWPORT, viewport);

			// OpenGL stores the bottom-left corner, but XNA
			// stores the upper-left corner, so we have to convert.
			viewport[1] = m_screenHeight - viewport[1] - viewport[3];
 			
			Viewport vp((float)viewport[0], (float)viewport[1], (float)viewport[2], (float)viewport[3]);
			return vp;
		}
		default:
			// we should never get here!
			return Viewport();
		}
	}

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

			// Use DECLARE_SAMPLER2D(binding point, name) to declare your samplers. This will allow explicit binding
			// points on supported hardware, and allow Nxna to query for the binding point on older hardware.

			// TODO: in order to support hardware that doesn't have the ARB_shading_language_420pack extension
			// we'll have to do a quick parse of the shader, looking for DECLARE_SAMPLER2D() and getting the
			// requested binding point, then use glGetUniformLocation() and build a map between the two.
			unsigned int s;
			if (strncmp((char*)bytecode, "#version", 8) == 0)
			{
				// The shader starts with #version, which means we can't add anything to it. Try it as-is.
				const GLchar* buffer[] = {
					(char*)bytecode
				};
				s = glCreateShaderProgramv(glType, 1, buffer);
			}
			else
			{
				const GLchar* buffer[] = { 
					"#version 410\n"
					"#extension GL_ARB_shading_language_420pack : require\n"
					"#define DECLARE_SAMPLER2D(b, n) layout(binding=b) uniform sampler2D n\n",
					(char*)bytecode 
				};
				s = glCreateShaderProgramv(glType, 2, buffer);
			}

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
		NXNA_VALIDATION_ASSERT(result != nullptr, "result cannot be null");

		memset(result, 0, sizeof(Texture2D));

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
			unsigned int mipLevels = desc->MipLevels;
			if (mipLevels == 0)
			{
				mipLevels = 1 + (unsigned int)floor(log2(desc->Width > desc->Height ? desc->Width : desc->Height));
			}

			glGenTextures(1, &result->OpenGL.Handle);
			glBindTexture(GL_TEXTURE_2D, result->OpenGL.Handle);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipLevels - 1);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			if (glTexStorage2D)
			{
				glTexStorage2D(GL_TEXTURE_2D, mipLevels, GL_RGBA8, desc->Width, desc->Height);
				if (desc->InitialData)
					glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, desc->Width, desc->Height, GL_RGBA, GL_UNSIGNED_BYTE, desc->InitialData);
			}
			else
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, desc->Width, desc->Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, desc->InitialData);
			}

			if (desc->MipLevels == 0)
				glGenerateMipmap(GL_TEXTURE_2D);
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
			if (state == nullptr)
				m_d3d11State.Context->OMSetBlendState(nullptr, nullptr, 0xffffffff);
			else if (m_d3d11State.CurrentBlendState != state->Direct3D11.State)
				m_d3d11State.Context->OMSetBlendState((ID3D11BlendState*)state->Direct3D11.State, nullptr, 0xffffffff);

			m_d3d11State.CurrentBlendState = state ? state->Direct3D11.State : nullptr;
		}
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			BlendStateDesc desc;
			if (state == nullptr)
				desc = NXNA_BLENDSTATEDESC_DEFAULT;
			else
				desc = state->OpenGL.Desc;

#define SET_GL_BLENDING_FOR_RT(rtIndex, state) \
				{ \
			if (m_oglState.CurrentBlendState.RenderTarget[rtIndex].BlendingEnabled != desc.RenderTarget[rtIndex].BlendingEnabled) \
											{ \
				if (desc.RenderTarget[rtIndex].BlendingEnabled) \
					glEnablei(GL_BLEND, rtIndex); \
													else \
					glDisablei(GL_BLEND, rtIndex); \
											} \
			\
			GLenum colorSrc, colorDest, alphaSrc, alphaDest; \
			CONVERT_GL_BLEND(desc.RenderTarget[rtIndex].ColorSourceBlend, colorSrc); \
			CONVERT_GL_BLEND(desc.RenderTarget[rtIndex].ColorDestinationBlend, colorDest); \
			CONVERT_GL_BLEND(desc.RenderTarget[rtIndex].AlphaSourceBlend, alphaSrc); \
			CONVERT_GL_BLEND(desc.RenderTarget[rtIndex].AlphaDestinationBlend, alphaDest); \
			glBlendFuncSeparatei(rtIndex, colorSrc, colorDest, alphaSrc, alphaDest); \
			\
			GLenum colorFunc, alphaFunc; \
			CONVERT_GL_BLEND_FUNC(desc.RenderTarget[rtIndex].ColorBlendFunction, colorFunc); \
			CONVERT_GL_BLEND_FUNC(desc.RenderTarget[rtIndex].AlphaBlendFunction, alphaFunc); \
			glBlendEquationSeparatei(rtIndex, colorFunc, alphaFunc); \
				}

			if (state == nullptr)
				goto allBuffers;			
			else if (m_oglState.CurrentBlendState.IndependentBlendEnabled != desc.IndependentBlendEnabled)
			{
				if (desc.IndependentBlendEnabled == true)
					goto allSeparate;
				else
					goto allBuffers;
			}
			else if (desc.IndependentBlendEnabled == false)
			{
				if (memcmp(&m_oglState.CurrentBlendState.RenderTarget[0], &desc.RenderTarget[0], sizeof(RenderTargetBlendStateDesc)) != 0)
					goto allBuffers;
				else
				{
					// no changes necesary
				}
			}
			else if (desc.IndependentBlendEnabled == true)
			{
				for (int i = 0; i < 8; i++)
				{
					if (memcmp(&m_oglState.CurrentBlendState.RenderTarget[i], &desc.RenderTarget[i], sizeof(RenderTargetBlendStateDesc)))
						SET_GL_BLENDING_FOR_RT(i, state);
				}
			}


		allBuffers:
			{
				if (state == nullptr || m_oglState.CurrentBlendState.RenderTarget[0].BlendingEnabled != desc.RenderTarget[0].BlendingEnabled)
				{
					if (desc.RenderTarget[0].BlendingEnabled)
						glEnable(GL_BLEND);
					else
						glDisable(GL_BLEND);
				}

				GLenum colorSrc, colorDest, alphaSrc, alphaDest;
				CONVERT_GL_BLEND(desc.RenderTarget[0].ColorSourceBlend, colorSrc);
				CONVERT_GL_BLEND(desc.RenderTarget[0].ColorDestinationBlend, colorDest);
				CONVERT_GL_BLEND(desc.RenderTarget[0].AlphaSourceBlend, alphaSrc);
				CONVERT_GL_BLEND(desc.RenderTarget[0].AlphaDestinationBlend, alphaDest);
				glBlendFuncSeparate(colorSrc, colorDest, alphaSrc, alphaDest);

				GLenum colorFunc, alphaFunc;
				CONVERT_GL_BLEND_FUNC(desc.RenderTarget[0].ColorBlendFunction, colorFunc);
				CONVERT_GL_BLEND_FUNC(desc.RenderTarget[0].AlphaBlendFunction, alphaFunc);
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

			m_oglState.CurrentBlendState = desc;
		}
			break;
		}
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
			if (state == nullptr || m_d3d11State.CurrentRasterizerState != state->Direct3D11.State)
				m_d3d11State.Context->RSSetState(state->Direct3D11.State);

			m_d3d11State.CurrentRasterizerState = state ? state->Direct3D11.State : nullptr;
		}
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			RasterizerStateDesc desc;
			if (state == nullptr)
				desc = NXNA_RASTERIZERSTATEDESC_DEFAULT;
			else
				desc = state->OpenGL.Desc;

			if (state == nullptr ||
				m_oglState.CurrentRasterizerState.CullingMode != desc.CullingMode ||
				m_oglState.CurrentRasterizerState.FrontCounterClockwise != desc.FrontCounterClockwise)
			{
				switch (desc.CullingMode)
				{
				case CullMode::None:
					glDisable(GL_CULL_FACE);
					break;
				case CullMode::CullBackFaces:
					glEnable(GL_CULL_FACE);
					glFrontFace(desc.FrontCounterClockwise ? GL_CCW : GL_CW);
					break;
				case CullMode::CullFrontFaces:
					glEnable(GL_CULL_FACE);
					glFrontFace(desc.FrontCounterClockwise ? GL_CW : GL_CCW);
					break;
				}
			}

			if (state == nullptr || 
				desc.ScissorTestEnabled != m_oglState.CurrentRasterizerState.ScissorTestEnabled)
			{
				if (desc.ScissorTestEnabled)
					glEnable(GL_SCISSOR_TEST);
				else
					glDisable(GL_SCISSOR_TEST);
			}

			if (state == nullptr ||
				desc.FillingMode != m_oglState.CurrentRasterizerState.FillingMode)
			{
				if (desc.FillingMode == FillMode::Solid)
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				else
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}

			m_oglState.CurrentRasterizerState = desc;
		}
			break;
		}
	}

	void GraphicsDevice::DestroyRasterizerState(RasterizerState* state)
	{
		switch(GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			state->Direct3D11.State->Release();
			state->Direct3D11.State = nullptr;

			if (m_d3d11State.CurrentRasterizerState == state->Direct3D11.State)
				m_d3d11State.CurrentRasterizerState = nullptr;
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

#define NXNA_CONVERT_COMPARISON_D3D11(s, d) { \
	switch(s) { \
	case CompareFunction::Always: d = D3D11_COMPARISON_ALWAYS; break; \
	case CompareFunction::Equal: d = D3D11_COMPARISON_EQUAL; break; \
	case CompareFunction::Greater: d = D3D11_COMPARISON_GREATER; break; \
	case CompareFunction::GreaterEqual: d = D3D11_COMPARISON_GREATER_EQUAL; break; \
	case CompareFunction::Less: d = D3D11_COMPARISON_LESS; break; \
	case CompareFunction::LessEqual: d = D3D11_COMPARISON_LESS_EQUAL; break; \
	case CompareFunction::Never: d = D3D11_COMPARISON_NEVER; break; \
	case CompareFunction::NotEqual: d = D3D11_COMPARISON_NOT_EQUAL; break; \
	} }

#define NXNA_CONVERT_STENCIL_OP_D3D11(s, d) { \
	switch(s) { \
	case StencilOperation::Decrement: d = D3D11_STENCIL_OP_DECR; break; \
	case StencilOperation::DecrementSaturation: d = D3D11_STENCIL_OP_DECR_SAT; break; \
	case StencilOperation::Increment: d = D3D11_STENCIL_OP_INCR; break; \
	case StencilOperation::IncrementSaturation: d = D3D11_STENCIL_OP_INCR_SAT; break; \
	case StencilOperation::Invert: d = D3D11_STENCIL_OP_INVERT; break; \
	case StencilOperation::Keep: d = D3D11_STENCIL_OP_KEEP; break; \
	case StencilOperation::Replace: d = D3D11_STENCIL_OP_REPLACE; break; \
	case StencilOperation::Zero: d = D3D11_STENCIL_OP_ZERO; break; \
	} }

#define NXNA_CONVERT_COMPARISON_OGL(s, d) { \
	switch(s) { \
	case CompareFunction::Always: d = GL_ALWAYS; break; \
	case CompareFunction::Equal: d = GL_EQUAL; break; \
	case CompareFunction::Greater: d = GL_GREATER; break; \
	case CompareFunction::GreaterEqual: d = GL_GEQUAL; break; \
	case CompareFunction::Less: d = GL_LESS; break; \
	case CompareFunction::LessEqual: d = GL_LEQUAL; break; \
	case CompareFunction::Never: d = GL_NEVER; break; \
	case CompareFunction::NotEqual: d = GL_NOTEQUAL; break; \
	} }

#define NXNA_CONVERT_STENCIL_OP_OGL(s, d) { \
	switch(s) { \
	case StencilOperation::Decrement: d = GL_DECR_WRAP; break; \
	case StencilOperation::DecrementSaturation: d = GL_DECR; break; \
	case StencilOperation::Increment: d = GL_INCR_WRAP; break; \
	case StencilOperation::IncrementSaturation: d = GL_INCR; break; \
	case StencilOperation::Invert: d = GL_INVERT; break; \
	case StencilOperation::Keep: d = GL_KEEP; break; \
	case StencilOperation::Replace: d = GL_REPLACE; break; \
	case StencilOperation::Zero: d = GL_ZERO; break; \
	} }

	NxnaResult GraphicsDevice::CreateDepthStencilState(const DepthStencilStateDesc* desc, DepthStencilState* result)
	{
		NXNA_VALIDATION_ASSERT(desc != nullptr, "desc cannot be null");
		NXNA_VALIDATION_ASSERT(result != nullptr, "result cannot be null");

		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			D3D11_DEPTH_STENCIL_DESC ddesc;
			ZeroMemory(&ddesc, sizeof(D3D11_DEPTH_STENCILOP_DESC));
			ddesc.DepthEnable = desc->DepthBufferEnabled;
			ddesc.DepthWriteMask = desc->DepthBufferWriteEnabled ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
			NXNA_CONVERT_COMPARISON_D3D11(desc->DepthBufferFunction, ddesc.DepthFunc);

			ddesc.StencilEnable = desc->StencilEnable;
			ddesc.StencilReadMask = 0xff;
			ddesc.StencilWriteMask = 0xff;

			// Stencil operations if pixel is front-facing.
			NXNA_CONVERT_STENCIL_OP_D3D11(desc->StencilFail, ddesc.FrontFace.StencilFailOp);
			NXNA_CONVERT_STENCIL_OP_D3D11(desc->StencilDepthBufferFail, ddesc.FrontFace.StencilDepthFailOp);
			NXNA_CONVERT_STENCIL_OP_D3D11(desc->StencilPass, ddesc.FrontFace.StencilPassOp);
			NXNA_CONVERT_COMPARISON_D3D11(desc->StencilFunction, ddesc.FrontFace.StencilFunc);

			// Stencil operations if pixel is back-facing.
			ddesc.BackFace.StencilFailOp = ddesc.FrontFace.StencilFailOp;
			ddesc.BackFace.StencilDepthFailOp = ddesc.FrontFace.StencilDepthFailOp;
			ddesc.BackFace.StencilPassOp = ddesc.FrontFace.StencilPassOp;
			ddesc.BackFace.StencilFunc = ddesc.FrontFace.StencilFunc;

			auto r = m_d3d11State.Device->CreateDepthStencilState(&ddesc, &result->Direct3D11.State);
			if (FAILED(r))
			{
				NXNA_SET_ERROR_DETAILS(r, "CreateDepthStencilState() failed");
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

	void GraphicsDevice::SetDepthStencilState(DepthStencilState* state)
	{
		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			if (state == nullptr)
				m_d3d11State.Context->OMSetDepthStencilState(nullptr, 0);
			else
				m_d3d11State.Context->OMSetDepthStencilState(state->Direct3D11.State, state->Direct3D11.ReferenceStencil);
		}
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			DepthStencilStateDesc newState;
			if (state != nullptr)
				newState = state->OpenGL.Desc;
			else
				newState = NXNA_DEPTHSTENCIL_DEFAULT;

			// depth buffer stuff
			if (state == nullptr || m_oglState.CurrentDepthStencilState.DepthBufferEnabled != newState.DepthBufferEnabled)
			{
				if (newState.DepthBufferEnabled)
					glEnable(GL_DEPTH_TEST);
				else
					glDisable(GL_DEPTH_TEST);
			}

			if (state == nullptr || m_oglState.CurrentDepthStencilState.DepthBufferFunction != newState.DepthBufferFunction)
			{
				GLenum f;
				NXNA_CONVERT_COMPARISON_OGL(newState.DepthBufferFunction, f);
				glDepthFunc(f);
			}

			if (state == nullptr || m_oglState.CurrentDepthStencilState.DepthBufferWriteEnabled != newState.DepthBufferWriteEnabled)
			{
				glDepthMask(newState.DepthBufferWriteEnabled ? GL_TRUE : GL_FALSE);
			}

			// stencil buffer stuff
			if (state == nullptr || m_oglState.CurrentDepthStencilState.StencilEnable != newState.StencilEnable)
			{
				if (newState.StencilEnable)
					glEnable(GL_STENCIL_TEST);
				else
					glDisable(GL_STENCIL_TEST);
			}

			if (state == nullptr ||
				m_oglState.CurrentDepthStencilState.StencilFunction != newState.StencilFunction ||
				m_oglState.CurrentDepthStencilState.ReferenceStencil != newState.ReferenceStencil)
			{
				GLenum f;
				NXNA_CONVERT_COMPARISON_OGL(newState.StencilFunction, f);
				glStencilFunc(f, newState.ReferenceStencil, 0xffffffff);
			}

			if (state == nullptr ||
				m_oglState.CurrentDepthStencilState.StencilFail != newState.StencilFail ||
				m_oglState.CurrentDepthStencilState.StencilDepthBufferFail != newState.StencilDepthBufferFail ||
				m_oglState.CurrentDepthStencilState.StencilPass != newState.StencilPass)
			{
				GLenum stencilFail, depthFail, stencilPass;
				NXNA_CONVERT_STENCIL_OP_OGL(newState.StencilFail, stencilFail);
				NXNA_CONVERT_STENCIL_OP_OGL(newState.StencilDepthBufferFail, depthFail);
				NXNA_CONVERT_STENCIL_OP_OGL(newState.StencilPass, stencilPass);
				glStencilOp(stencilFail, depthFail, stencilPass);
			}

			m_oglState.CurrentDepthStencilState = newState;
		}
			break;
		}
	}

	void GraphicsDevice::DestroyDepthStencilState(DepthStencilState* state)
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

#define NXNA_CONVERT_TEXTURE_ADDRESS_OGL(s, d) { \
	switch(s) { \
	case TextureAddressMode::Clamp: d = GL_CLAMP_TO_EDGE; break; \
	case TextureAddressMode::Mirror: d = GL_MIRRORED_REPEAT; break; \
	case TextureAddressMode::Wrap: d = GL_REPEAT; break; \
	case TextureAddressMode::Border: d = GL_CLAMP_TO_BORDER; break; \
	case TextureAddressMode::MirrorOnce: d = GL_MIRROR_CLAMP_TO_EDGE; break; \
		} }

#define NXNA_CONVERT_TEXTURE_ADDRESS_D3D11(s, d) { \
	switch(s) { \
	case TextureAddressMode::Clamp: d = D3D11_TEXTURE_ADDRESS_CLAMP; break; \
	case TextureAddressMode::Mirror: d = D3D11_TEXTURE_ADDRESS_MIRROR; break; \
	case TextureAddressMode::Wrap: d = D3D11_TEXTURE_ADDRESS_WRAP; break; \
	case TextureAddressMode::Border: d = D3D11_TEXTURE_ADDRESS_BORDER; break; \
	case TextureAddressMode::MirrorOnce: d = D3D11_TEXTURE_ADDRESS_MIRROR_ONCE; break; \
	} }

	NxnaResult GraphicsDevice::CreateSamplerState(const SamplerStateDesc* desc, SamplerState* result)
	{
		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			D3D11_SAMPLER_DESC sd = {};
			switch (desc->Filter)
			{
			case TextureFilter::Point:
				sd.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
				break;
			case TextureFilter::Linear:
				sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
				break;
			case TextureFilter::Anisotropic:
				sd.Filter = D3D11_FILTER_ANISOTROPIC;
				break;
			case TextureFilter::LinearMipPoint:
				sd.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
				break;
			case TextureFilter::PointMipLinear:
				sd.Filter = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
				break;
			case TextureFilter::MinLinearMagPointMipLinear:
				sd.Filter = D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
				break;
			case TextureFilter::MinLinearMagPointMipPoint:
				sd.Filter = D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
				break;
			case TextureFilter::MinPointMagLinearMipLinear:
				sd.Filter = D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
				break;
			case TextureFilter::MinPointMagLinearMipPoint:
				sd.Filter = D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
				break;
			}

			NXNA_CONVERT_TEXTURE_ADDRESS_D3D11(desc->AddressU, sd.AddressU);
			NXNA_CONVERT_TEXTURE_ADDRESS_D3D11(desc->AddressV, sd.AddressV);
			NXNA_CONVERT_TEXTURE_ADDRESS_D3D11(desc->AddressW, sd.AddressW);
			sd.MipLODBias = desc->MipLODBias;
			sd.MaxAnisotropy = desc->MaxAnisotropy;
			sd.BorderColor[0] = desc->BorderColor[0];
			sd.BorderColor[1] = desc->BorderColor[1];
			sd.BorderColor[2] = desc->BorderColor[2];
			sd.BorderColor[3] = desc->BorderColor[3];
			sd.MinLOD = desc->MinLOD;
			sd.MaxLOD = desc->MaxLOD;
			sd.ComparisonFunc = D3D11_COMPARISON_NEVER; // TODO: support comparison modes

			auto r = m_d3d11State.Device->CreateSamplerState(&sd, &result->Direct3D11.State);
			if (FAILED(r))
			{
				NXNA_SET_ERROR_DETAILS(r, "CreateSamplerState() failed");
				return NxnaResult::UnknownError;
			}

			return NxnaResult::Success;
		}
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			glGenSamplers(1, &result->OpenGL.Handle);
			GLenum minFilter, magFilter;
			switch (desc->Filter)
			{
			case TextureFilter::Point:
				minFilter = GL_NEAREST_MIPMAP_NEAREST;
				magFilter = GL_NEAREST;
				break;
			case TextureFilter::Linear:
				minFilter = GL_LINEAR_MIPMAP_LINEAR;
				magFilter = GL_LINEAR;
				break;
			case TextureFilter::Anisotropic:
				minFilter = GL_LINEAR_MIPMAP_LINEAR;
				magFilter = GL_LINEAR;
				break;
			case TextureFilter::LinearMipPoint:
				minFilter = GL_LINEAR_MIPMAP_NEAREST;
				magFilter = GL_LINEAR;
				break;
			case TextureFilter::PointMipLinear:
				minFilter = GL_NEAREST_MIPMAP_LINEAR;
				magFilter = GL_NEAREST;
				break;
			case TextureFilter::MinLinearMagPointMipLinear:
				minFilter = GL_LINEAR_MIPMAP_LINEAR;
				magFilter = GL_NEAREST;
				break;
			case TextureFilter::MinLinearMagPointMipPoint:
				minFilter = GL_LINEAR_MIPMAP_NEAREST;
				magFilter = GL_NEAREST;
				break;
			case TextureFilter::MinPointMagLinearMipLinear:
				minFilter = GL_NEAREST_MIPMAP_LINEAR;
				magFilter = GL_NEAREST;
				break;
			case TextureFilter::MinPointMagLinearMipPoint:
				minFilter = GL_NEAREST_MIPMAP_NEAREST;
				magFilter = GL_LINEAR;
				break;
			}
			glSamplerParameteri(result->OpenGL.Handle, GL_TEXTURE_MIN_FILTER, minFilter);
			glSamplerParameteri(result->OpenGL.Handle, GL_TEXTURE_MAG_FILTER, magFilter);

			GLenum wrapU, wrapV, wrapW;
			NXNA_CONVERT_TEXTURE_ADDRESS_OGL(desc->AddressU, wrapU);
			NXNA_CONVERT_TEXTURE_ADDRESS_OGL(desc->AddressV, wrapV);
			NXNA_CONVERT_TEXTURE_ADDRESS_OGL(desc->AddressW, wrapW);
			glSamplerParameteri(result->OpenGL.Handle, GL_TEXTURE_WRAP_S, wrapU);
			glSamplerParameteri(result->OpenGL.Handle, GL_TEXTURE_WRAP_T, wrapV);
			glSamplerParameteri(result->OpenGL.Handle, GL_TEXTURE_WRAP_R, wrapW);
			glSamplerParameterf(result->OpenGL.Handle, GL_TEXTURE_LOD_BIAS, desc->MipLODBias);
			glSamplerParameteri(result->OpenGL.Handle, GL_TEXTURE_MAX_ANISOTROPY_EXT, desc->MaxAnisotropy);
			glSamplerParameterfv(result->OpenGL.Handle, GL_TEXTURE_BORDER_COLOR, desc->BorderColor);
			glSamplerParameterf(result->OpenGL.Handle, GL_TEXTURE_MIN_LOD, desc->MinLOD);
			glSamplerParameterf(result->OpenGL.Handle, GL_TEXTURE_MAX_LOD, desc->MaxLOD);


			// TODO: verify that the driver supports anisotropic filtering, return NxnaResult::NotSupported if not and they request anything > 1
			// TODO: if the user requests MirrorOnce then verify that the driver supports it before requesting it, return NxnaResult::NotSupported if not
			return NxnaResult::Success;
		}
			break;
		default:
			return NxnaResult::NotSupported;
		}
	}

	void GraphicsDevice::SetSamplerState(unsigned int slot, SamplerState* sampler)
	{
		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			if (sampler == nullptr)
			{
				m_d3d11State.Context->PSSetSamplers(slot, 1, nullptr);
			}
			else
			{
				m_d3d11State.Context->PSSetSamplers(slot, 1, &sampler->Direct3D11.State);
			}
		}
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			if (sampler == nullptr)
			{
				glBindSampler(slot, m_oglState.DefaultSamplerState);
			}
			else
			{
				glBindSampler(slot, sampler->OpenGL.Handle);
			}
		}
			break;
		}
	}

	void GraphicsDevice::SetSamplerStates(unsigned int startSlot, unsigned int numSamplers, SamplerState* const* samplers)
	{
		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			if (samplers == nullptr)
			{
				m_d3d11State.Context->PSSetSamplers(startSlot, numSamplers, nullptr);
			}
			else
			{
				for (unsigned int i = 0; i < numSamplers; i++)
				{
					if (samplers[i] == nullptr)
						m_d3d11State.Context->PSSetSamplers(startSlot + i, 1, nullptr);
					else
						m_d3d11State.Context->PSSetSamplers(startSlot + i, 1, &samplers[i]->Direct3D11.State);
				}
			}
		}
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			if (samplers == nullptr)
			{
				for (unsigned int i = startSlot; i < m_caps.MaxSamplerCount; i++)
				{
					glBindSampler(i, m_oglState.DefaultSamplerState);
				}
			}
			else
			{
				for (unsigned int i = 0; i < numSamplers; i++)
				{
					if (samplers[i] == nullptr)
						glBindSampler(startSlot + i, m_oglState.DefaultSamplerState);
					else
						glBindSampler(startSlot + i, samplers[i]->OpenGL.Handle);
				}
			}
		}
			break;
		}
	}

	void GraphicsDevice::DestroySamplerState(SamplerState* state)
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
			glDeleteSamplers(1, &state->OpenGL.Handle);
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

	void GraphicsDevice::DrawPrimitives(PrimitiveType primitiveType, int startIndex, int primitiveCount)
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

			// TODO
			//m_d3d11State.Context->DrawIndexed(indexCount, startIndex, baseVertex);
		}
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			GLenum glPrimitiveType;
			if (primitiveType == PrimitiveType::TriangleStrip)
				glPrimitiveType = GL_TRIANGLE_STRIP;
			else
				glPrimitiveType = GL_TRIANGLES;

			glDrawArrays(glPrimitiveType, startIndex, primitiveCount * 3);
		}
		}
	}

	void GraphicsDevice::ClearColor(Color color)
	{
		float rgba[] = { color.R / 255.0f, color.G / 255.0f, color.B / 255.0f, color.A / 255.0f };

		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			m_d3d11State.Context->ClearRenderTargetView(m_d3d11State.RenderTargetView, rgba);
		}
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			glClearBufferfv(GL_COLOR, 0, rgba);
		}
			break;
		}
	}

	void GraphicsDevice::ClearColor(float r, float g, float b, float a)
	{
		float rgba[] = { r, g, b, a };
		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			m_d3d11State.Context->ClearRenderTargetView(m_d3d11State.RenderTargetView, rgba);
		}
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			glClearBufferfv(GL_COLOR, 0, rgba);
		}
			break;
		}
	}
	
	void GraphicsDevice::ClearColor(const float* colorRGBA4f)
	{
		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			m_d3d11State.Context->ClearRenderTargetView(m_d3d11State.RenderTargetView, colorRGBA4f);
		}
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			glClearBufferfv(GL_COLOR, 0, colorRGBA4f);
		}
			break;
		}
	}

	void GraphicsDevice::ClearDepthStencil(bool clearDepth, bool clearStencil, float depthValue, int stencilValue)
	{
		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			UINT flags = 0;
			if (clearDepth) flags |= D3D11_CLEAR_DEPTH;
			if (clearStencil) flags |= D3D11_CLEAR_STENCIL;
			m_d3d11State.Context->ClearDepthStencilView(m_d3d11State.DepthStencilView, flags , depthValue, (UINT8)stencilValue);
		}
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			// force depth writing on, otherwise the clear won't have any effect
			if (m_oglState.CurrentDepthStencilState.DepthBufferWriteEnabled == false)
			{
				glDepthMask(GL_TRUE);
			}

			glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			if (clearDepth && clearStencil)
				glClearBufferfi(GL_DEPTH_STENCIL, 0, depthValue, stencilValue);
			else if (clearDepth)
				glClearBufferfv(GL_DEPTH, 0, &depthValue);
			else if (clearStencil)
				glClearBufferiv(GL_STENCIL, 0, &stencilValue);

			// restore the depth mask
			if (m_oglState.CurrentDepthStencilState.DepthBufferWriteEnabled == false)
			{
				glDepthMask(GL_FALSE);
			}
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
