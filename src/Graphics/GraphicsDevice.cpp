#include "GraphicsDevice.h"
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

	Vector3 Viewport::Project(const Vector3& source,
		const Matrix& transform)
	{
		Nxna::Vector4 result;
		result.X = source.X;
		result.Y = source.Y;
		result.Z = source.Z;
		result.W = 1.0f;

		Nxna::Vector4::Transform(result, transform, result);

		// perspective divide
		result = result / result.W;

		Nxna::Vector3 result3;
		result3.X = (result.X + 1.0f) * 0.5f * (float)Width + (float)X;
		result3.Y = (-result.Y + 1.0f) * 0.5f * (float)Height + (float)Y;

		// TODO: i doubt the Z coords are correct since they're not taking into account
		// the clipping plains
		result3.Z = result.Z;

		return result3;
	}

	Nxna::Vector3 Viewport::Unproject(const Nxna::Vector3& source,
		const Nxna::Matrix& projection,
		const Nxna::Matrix& view,
		const Nxna::Matrix& world)
	{
		Nxna::Vector4 result;
		result.X = ((source.X - X) * 2 / Width) - 1;
		result.Y = 1 - ((source.Y - Y) * 2 / Height);
		result.Z = source.Z;
		result.W = 1.0f;

		Nxna::Matrix invProj, invView, invWorld;
		Nxna::Matrix::Invert(projection, invProj);
		Nxna::Matrix::Invert(view, invView);
		Nxna::Matrix::Invert(world, invWorld);

		Nxna::Vector4::Transform(result, invProj, result);
		Nxna::Vector4::Transform(result, invView, result);
		Nxna::Vector4::Transform(result, invWorld, result);

		result = result / result.W;

		return Nxna::Vector3(result.X, result.Y, result.Z);
}

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

		memset(&result->m_shaderPipeline, 0, sizeof(ShaderPipeline));

		switch (params->Type)
		{
		case GraphicsDeviceType::OpenGl41:
		{
			NXNA_MEMSET(&result->m_oglState, 0, sizeof(OpenGlDeviceState));

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
			glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, (GLint*)&result->m_caps.MaxRenderTargets);
			if (result->m_caps.MaxRenderTargets > 8) result->m_caps.MaxRenderTargets = 8; // the hardware supports more than Nxna does! :(
			result->m_caps.TextureOriginUpperLeft = false;

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

			result->m_oglState.CurrentFBO = 0;
			result->m_oglState.CurrentFBOHeight = result->m_screenHeight;

			// create a default VAO
			glGenVertexArrays(1, &result->m_oglState.DefaultVAO);

			// mark all the states dirty
			result->m_oglState.CurrentVertexBuffersDirty = true;
			result->m_oglState.CurrentIndexBufferDirty = true;
			result->m_oglState.CurrentInputElementsDirty = true;

			result->m_oglState.CurrentIndexBuffer = 0;
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
			result->m_caps.MaxRenderTargets = D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT;
			result->m_caps.TextureOriginUpperLeft = true;

			result->m_d3d11State.Device = params->Direct3D11.Device;
			result->m_d3d11State.Context = params->Direct3D11.DeviceContext;
			result->m_d3d11State.DefaultRenderTargetView = params->Direct3D11.RenderTargetView;
			result->m_d3d11State.DefaultDepthStencilView = params->Direct3D11.DepthStencilView;
			result->m_d3d11State.SwapChain = params->Direct3D11.SwapChain;

			memset(result->m_d3d11State.CurrentRenderTargetViews, 0, sizeof(result->m_d3d11State.CurrentRenderTargetViews));
			result->m_d3d11State.CurrentRenderTargetViews[0] = params->Direct3D11.RenderTargetView;
			result->m_d3d11State.CurrentDepthStencilView = params->Direct3D11.DepthStencilView;
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
		default:
			;
		}

#ifndef NDEBUG
		static_assert((int)GraphicsDeviceType::LAST == g_numDeviceTypes, "m_numDeviceTypes is incorrect");
#endif
	}

	void GraphicsDevice::OnScreenSizeChanged(int newWidth, int newHeight)
	{
		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			// TODO
		}
		break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			m_screenWidth = newWidth;
			m_screenHeight = newHeight;

			if (m_oglState.CurrentFBO == 0)
				m_oglState.CurrentFBOHeight = m_screenHeight;
		}
		default:
			;
		}
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
			int y2 = m_oglState.CurrentFBOHeight - (int)(height + y);

			glViewport((int)x, y2, (int)width, (int)height);
			glDepthRange(minDepth, maxDepth);
		}
			break;
		default:
			;
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
			int y2 = m_oglState.CurrentFBOHeight - (int)(viewport.Height + viewport.Y);

			glViewport((int)viewport.X, y2, (int)viewport.Width, (int)viewport.Height);
		}
		default:
			;
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
					"#define DECLARE_SAMPLER2D(b, n) layout(binding=b) uniform sampler2D n\n"
					"#define DECLARE_SAMPLER2DARRAY(b, n) layout(binding=b) uniform sampler2DArray n\n"
					"#define DECLARE_SAMPLERCUBE(b, n) layout(binding=b) uniform samplerCube n\n",
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
		default:
			return NxnaResult::NotSupported;
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
		default:
			;
		}
	}

	NxnaResult GraphicsDevice::CreateTexture2D(const TextureCreationDesc* desc, const SubresourceData* initialData, Texture2D* result)
	{
		NXNA_VALIDATION_ASSERT(desc != nullptr, "desc cannot be null");
		NXNA_VALIDATION_ASSERT(result != nullptr, "result cannot be null");

		if (desc->Type == TextureType::TextureCube && desc->ArraySize != 6)
		{
			// when creating a cube map we must have all 6 sides
			return NxnaResult::InvalidArgument;
		}

		memset(result, 0, sizeof(Texture2D));

		unsigned int arraySize = desc->ArraySize;
		if (arraySize == 0) arraySize = 1;

		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			static_assert(sizeof(SubresourceData) == sizeof(D3D11_SUBRESOURCE_DATA), "SubresourceData is unexpected size");

			HRESULT r;
			D3D11_TEXTURE2D_DESC dtdesc;
			ZeroMemory(&dtdesc, sizeof(D3D11_TEXTURE2D_DESC));

			dtdesc.ArraySize = arraySize;
			dtdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			dtdesc.Width = desc->Width;
			dtdesc.Height = desc->Height;
			dtdesc.MipLevels = desc->MipLevels;
			dtdesc.SampleDesc.Count = 1;
			dtdesc.SampleDesc.Quality = 0;
			dtdesc.Usage = D3D11_USAGE_DEFAULT;
			dtdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			if (desc->Flags & (int)TextureCreationFlags::AllowRenderTargetColorAttachment)
				dtdesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
			if (desc->Flags & (int)TextureCreationFlags::AllowRenderTargetDepthAttachment)
				dtdesc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
			dtdesc.CPUAccessFlags = 0;
			dtdesc.MiscFlags = 0; // TODO: support cube maps

			if (initialData != nullptr)
			{
				r = m_d3d11State.Device->CreateTexture2D(&dtdesc, (D3D11_SUBRESOURCE_DATA*)initialData, &result->Direct3D11.m_texture);
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
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D; // TODO: make this based on the texture type
			srvDesc.Texture2D.MipLevels = -1;

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
			if (desc->Type == TextureType::TextureCube)
			{
				result->OpenGL.IsCubeMap = true;
					
				glBindTexture(GL_TEXTURE_CUBE_MAP, result->OpenGL.Handle);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, mipLevels - 1);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				assert(arraySize == 6);
				for (unsigned int i = 0; i < arraySize; i++)
				{
					if (initialData)
						glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, desc->Width, desc->Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, initialData[i].Data);
					else
						glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, desc->Width, desc->Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
				}

				if (desc->MipLevels == 0)
					glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
			}
			else if (desc->Type == TextureType::Texture2DArray)
			{
				result->OpenGL.IsArray = true;

				glBindTexture(GL_TEXTURE_2D_ARRAY, result->OpenGL.Handle);
				glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BASE_LEVEL, 0);
				glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, mipLevels - 1);
				glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				if (glTexStorage3D)
				{
					glTexStorage3D(GL_TEXTURE_2D_ARRAY, mipLevels, GL_RGBA8, desc->Width, desc->Height, arraySize);
				}
				else
				{
					glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, desc->Width, desc->Height, arraySize, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
				}

				if (initialData)
				{
					for (unsigned int i = 0; i < arraySize; i++)
						glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, desc->Width, desc->Height, 1, GL_RGBA, GL_UNSIGNED_BYTE, initialData[i].Data);
				}

				if (desc->MipLevels == 0)
					glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
			}
			else
			{
				result->OpenGL.IsArray = false;

				glBindTexture(GL_TEXTURE_2D, result->OpenGL.Handle);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipLevels - 1);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				if (glTexStorage2D)
				{
					glTexStorage2D(GL_TEXTURE_2D, mipLevels, GL_RGBA8, desc->Width, desc->Height);
					if (initialData)
						glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, desc->Width, desc->Height, GL_RGBA, GL_UNSIGNED_BYTE, initialData[0].Data);
				}
				else
				{
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, desc->Width, desc->Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, initialData[0].Data);
				}

				if (desc->MipLevels == 0)
					glGenerateMipmap(GL_TEXTURE_2D);
			}
		}
			break;
		default:
			return NxnaResult::NotSupported;
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

			if (texture->OpenGL.IsArray)
				glBindTexture(GL_TEXTURE_2D_ARRAY, texture->OpenGL.Handle);
			else if (texture->OpenGL.IsCubeMap)
				glBindTexture(GL_TEXTURE_CUBE_MAP, texture->OpenGL.Handle);
			else
				glBindTexture(GL_TEXTURE_2D, texture->OpenGL.Handle);

			break;
		default:
			;
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

				iedesc[i].InputSlot = e[i].InputSlot;
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
			{
				glUseProgramStages(pipeline, GL_VERTEX_SHADER_BIT, desc->VertexShader->OpenGL.Handle);
				result->OpenGL.VertexShader = desc->VertexShader->OpenGL.Handle;
			}
			else
			{
				result->OpenGL.VertexShader = 0;
			}

			if (desc->PixelShader != nullptr)
			{
				glUseProgramStages(pipeline, GL_FRAGMENT_SHADER_BIT, desc->PixelShader->OpenGL.Handle);
				result->OpenGL.FragmentShader = desc->PixelShader->OpenGL.Handle;
			}
			else
			{
				result->OpenGL.FragmentShader = 0;
			}

			result->OpenGL.Pipeline = pipeline;
			

			int numElements = desc->NumElements;
			if (numElements > 16) numElements = 16;
			result->OpenGL.NumElements = numElements;
			memcpy(result->OpenGL.VertexElements, desc->VertexElements, sizeof(InputElement) * numElements);
		}
			break;
		default:
			return NxnaResult::NotSupported;
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
					if (m_shaderPipeline.OpenGL.Pipeline != pipeline->OpenGL.Pipeline)
						glBindProgramPipeline(pipeline->OpenGL.Pipeline);

					m_oglState.CurrentInputElementsDirty = true;
				}
				else
				{
					glBindProgramPipeline(0);
				}
			}
				break;
			default:
				;
			}
		}

		m_shaderPipeline = *pipeline;
	}

	void GraphicsDevice::DestroyShaderPipeline(ShaderPipeline* pipeline)
	{
		NXNA_VALIDATION_ASSERT(pipeline != nullptr, "pipeline cannot be null");
		

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
			NXNA_VALIDATION_ASSERT(m_shaderPipeline.OpenGL.Pipeline != pipeline->OpenGL.Pipeline, "pipeline cannot be the current ShaderPipeline");

			glDeleteProgramPipelines(1, &pipeline->OpenGL.Pipeline);
			pipeline->OpenGL.Pipeline = 0;
		}
			break;
		default:
			;
		}
	}

#define CONVERT_D3D11_BLEND(blend, dest) \
	switch(blend) { \
		case Blend::Zero: dest = D3D11_BLEND_ZERO; break;\
		case Blend::One: dest = D3D11_BLEND_ONE; break; \
		case Blend::SourceColor: dest = D3D11_BLEND_SRC_COLOR; break; \
		case Blend::InverseSourceColor: dest = D3D11_BLEND_INV_SRC_COLOR; break; \
		case Blend::DestinationColor: dest = D3D11_BLEND_DEST_COLOR; break; \
		case Blend::InverseDestinationColor: dest = D3D11_BLEND_INV_DEST_COLOR; break; \
		case Blend::SourceAlpha: dest = D3D11_BLEND_SRC_ALPHA; break; \
		case Blend::DestinationAlpha: dest = D3D11_BLEND_DEST_ALPHA; break; \
		case Blend::InverseSourceAlpha: dest = D3D11_BLEND_INV_SRC_ALPHA; break; \
		case Blend::InverseDestinationAlpha: dest = D3D11_BLEND_INV_DEST_ALPHA; break; \
		case Blend::Source1Color: dest = D3D11_BLEND_SRC1_COLOR; break; \
		case Blend::InverseSource1Color: dest = D3D11_BLEND_INV_SRC1_COLOR; break; \
		case Blend::Source1Alpha: dest = D3D11_BLEND_SRC1_ALPHA; break; \
		case Blend::InverseSource1Alpha: dest = D3D11_BLEND_INV_SRC1_ALPHA; break; \
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
		case Blend::SourceColor: dest = GL_SRC_COLOR; break; \
		case Blend::InverseSourceColor: dest = GL_ONE_MINUS_SRC_COLOR; break; \
		case Blend::DestinationColor: dest =  GL_DST_COLOR; break; \
		case Blend::InverseDestinationColor: dest = GL_ONE_MINUS_DST_COLOR; break; \
		case Blend::SourceAlpha: dest = GL_SRC_ALPHA; break; \
		case Blend::DestinationAlpha: dest = GL_DST_ALPHA; break; \
		case Blend::InverseSourceAlpha: dest = GL_ONE_MINUS_SRC_ALPHA; break; \
		case Blend::InverseDestinationAlpha: dest = GL_ONE_MINUS_DST_ALPHA; break; \
		case Blend::Source1Color: dest = GL_SRC1_COLOR; break; \
		case Blend::InverseSource1Color: dest =  GL_ONE_MINUS_SRC1_COLOR; break; \
		case Blend::Source1Alpha: dest =  GL_SRC1_ALPHA; break; \
		case Blend::InverseSource1Alpha: dest =  GL_ONE_MINUS_SRC1_ALPHA; break; \
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
		default:
			return NxnaResult::NotSupported;
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
		default:
			;
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
		default:
			;
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
		default:
			return NxnaResult::NotSupported;
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
		default:
			;
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
		default:
			;
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
		default:
			return NxnaResult::NotSupported;
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
		default:
			;
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
		default:
			;
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
		default:
			;
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
		default:
			;
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
		default:
			;
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
			m_oglState.CurrentIndexBuffer = indices.OpenGL.Buffer;
			m_oglState.CurrentIndexBufferDirty = true;
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
		result->ByteLength = desc->ByteLength;
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
			vbdesc.ByteWidth = desc->ByteLength;
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
			result->OpenGL.ByteLength = desc->ByteLength;

			glGenBuffers(1, &result->OpenGL.Buffer);
			glBindBuffer(GL_ARRAY_BUFFER, result->OpenGL.Buffer);
			glBufferData(GL_ARRAY_BUFFER, result->OpenGL.ByteLength, nullptr, GL_STATIC_DRAW);
			if (desc->InitialData != nullptr)
				glBufferSubData(GL_ARRAY_BUFFER, 0, desc->InitialDataByteCount, desc->InitialData);

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

	void GraphicsDevice::SetVertexBuffer(VertexBuffer* vertexBuffer, unsigned int offset, unsigned int stride)
	{
		SetVertexBuffers(0, 1, &vertexBuffer, &offset, &stride);
	}

	void GraphicsDevice::SetVertexBuffers(unsigned int startSlot, unsigned int numBuffers, VertexBuffer** vertexBuffers, unsigned int* offsets, unsigned int* stride)
	{
		// TODO: make sure the startSlot and numBuffers are withing valid bounds

		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			for (unsigned int i = 0; i < numBuffers; i++)
				m_d3d11State.Context->IASetVertexBuffers(startSlot + i, 1, &vertexBuffers[i]->Direct3D11.Buffer, &stride[i], &offsets[i]);
		}
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			for (unsigned int i = 0; i < numBuffers; i++)
			{
				if (vertexBuffers[i] == nullptr)
				{
					if (m_oglState.CurrentVertexBufferActive[startSlot + i])
					{
						m_oglState.CurrentVertexBufferActive[startSlot + i] = false;
						m_oglState.CurrentVertexBufferDirty[startSlot + i] = true;
						m_oglState.CurrentVertexBuffersDirty = true;
					}
				}
				else if (m_oglState.CurrentVertexBufferDirty[startSlot + i] == true ||
					m_oglState.CurrentVertexBufferActive[startSlot + i] == false ||
					vertexBuffers[i]->OpenGL.Buffer != m_oglState.CurrentVertexBuffers[startSlot + i].Buffer ||
					offsets[i] != m_oglState.CurrentVertexBuffers[startSlot + i].Offset ||
					stride[i] != m_oglState.CurrentVertexBuffers[startSlot + i].Stride)
				{
					m_oglState.CurrentVertexBuffers[startSlot + i].Buffer = vertexBuffers[i]->OpenGL.Buffer;
					m_oglState.CurrentVertexBuffers[startSlot + i].Offset = offsets[i];
					m_oglState.CurrentVertexBuffers[startSlot + i].Stride = stride[i];
					m_oglState.CurrentVertexBufferActive[startSlot + i] = true;
					m_oglState.CurrentVertexBufferDirty[startSlot + i] = true;
					m_oglState.CurrentVertexBuffersDirty = true;
				}
			}
		}
			break;
		default:
			NXNA_SET_ERROR_DETAILS(0, "Unsupported graphics device type");
		}
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
			break;
		default:
			return NxnaResult::NotSupported;
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
			break;
		default:
			;
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
			break;
		default:
			;
		}
	}

	NxnaResult GraphicsDevice::CreateRenderTarget(const RenderTargetDesc* desc, RenderTarget* result)
	{
		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			memset(result, 0, sizeof(RenderTarget));

			if (desc->DepthFormatType != DepthFormat::None)
			{
				D3D11_TEXTURE2D_DESC depthBufferDesc;
				ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
				depthBufferDesc.Width = desc->Width;
				depthBufferDesc.Height = desc->Height;
				depthBufferDesc.MipLevels = 1;
				depthBufferDesc.ArraySize = 1;
				if (desc->DepthFormatType == DepthFormat::Depth16)
					depthBufferDesc.Format = DXGI_FORMAT_D16_UNORM;
				else if (desc->DepthFormatType == DepthFormat::Depth24 || desc->DepthFormatType == DepthFormat::Depth24Stencil8)
					depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
				depthBufferDesc.SampleDesc.Count = 1;
				depthBufferDesc.SampleDesc.Quality = 0;
				depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
				depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
				depthBufferDesc.CPUAccessFlags = 0;
				depthBufferDesc.MiscFlags = 0;

				auto r = m_d3d11State.Device->CreateTexture2D(&depthBufferDesc, nullptr, &result->Direct3D11.DefaultDepthStencilTexture);
				if (FAILED(r))
				{
					NXNA_SET_ERROR_DETAILS(r, "CreateTexture2D() failed when creating default depth texture");
					return NxnaResult::UnknownError;
				}

				r = m_d3d11State.Device->CreateDepthStencilView(result->Direct3D11.DefaultDepthStencilTexture, nullptr, &result->Direct3D11.DefaultDepthStencilView);
				if (FAILED(r))
				{
					result->Direct3D11.DefaultDepthStencilTexture->Release();
					NXNA_SET_ERROR_DETAILS(r, "CreateDepthStencilView() failed when creating default depth texture");
					return NxnaResult::UnknownError;
				}
			}

			return NxnaResult::Success;
		}
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			glGenFramebuffers(1, &result->OpenGL.FBO);
			result->OpenGL.Height = desc->Height;

			if (desc->DepthFormatType != DepthFormat::None)
			{
				glGenRenderbuffers(1, &result->OpenGL.DefaultDepthStencilTexture);
				glBindRenderbuffer(GL_RENDERBUFFER, result->OpenGL.DefaultDepthStencilTexture);

				if (desc->DepthFormatType == DepthFormat::Depth16)
					glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, desc->Width, desc->Height);
				else if (desc->DepthFormatType == DepthFormat::Depth24)
					glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, desc->Width, desc->Height);
				else if (desc->DepthFormatType == DepthFormat::Depth24Stencil8)
					glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, desc->Width, desc->Height);

				if (glNamedFramebufferRenderbuffer)
					glNamedFramebufferRenderbuffer(result->OpenGL.FBO, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, result->OpenGL.DefaultDepthStencilTexture);
				else
				{
					glBindFramebuffer(GL_FRAMEBUFFER, result->OpenGL.FBO);
					glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, result->OpenGL.DefaultDepthStencilTexture);
				}

				if (desc->DepthFormatType == DepthFormat::Depth24Stencil8)
				{
					if (glNamedFramebufferRenderbuffer)
						glFramebufferRenderbuffer(result->OpenGL.FBO, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, result->OpenGL.DefaultDepthStencilTexture);
					else
					{
						glBindFramebuffer(GL_FRAMEBUFFER, result->OpenGL.FBO);
						glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, result->OpenGL.DefaultDepthStencilTexture);
					}
				}
				else
				{
					if (glNamedFramebufferRenderbuffer)
						glNamedFramebufferRenderbuffer(result->OpenGL.FBO, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, result->OpenGL.DefaultDepthStencilTexture);
					else
					{
						glBindFramebuffer(GL_FRAMEBUFFER, result->OpenGL.FBO);
						glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, result->OpenGL.DefaultDepthStencilTexture);
					}
				}

				if (!glNamedFramebufferRenderbuffer)
					glBindFramebuffer(GL_FRAMEBUFFER, m_oglState.CurrentFBO);
			}

			return NxnaResult::Success;
		}
			break;
		default:
			return NxnaResult::NotSupported;
		}

		return NxnaResult::NotSupported;
	}

	NxnaResult GraphicsDevice::BindRenderTarget(RenderTarget* renderTarget)
	{
		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			if (renderTarget == nullptr)
			{
				m_d3d11State.Context->OMSetRenderTargets(1, &m_d3d11State.DefaultRenderTargetView, m_d3d11State.DefaultDepthStencilView);

				memset(m_d3d11State.CurrentRenderTargetViews, 0, sizeof(m_d3d11State.CurrentRenderTargetViews));
				m_d3d11State.CurrentRenderTargetViews[0] = m_d3d11State.DefaultRenderTargetView;
				m_d3d11State.CurrentDepthStencilView = m_d3d11State.DefaultDepthStencilView;
			}
			else
			{
				m_d3d11State.Context->OMSetRenderTargets(8, renderTarget->Direct3D11.ColorAttachments, renderTarget->Direct3D11.DepthAttachment);
				memcpy(m_d3d11State.CurrentRenderTargetViews, renderTarget->Direct3D11.ColorAttachments, sizeof(m_d3d11State.CurrentRenderTargetViews));;
				m_d3d11State.CurrentDepthStencilView = renderTarget->Direct3D11.DepthAttachment;
			}

			return NxnaResult::Success;
		}
		break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			if (renderTarget == nullptr)
			{
				m_oglState.CurrentFBO = 0;
				m_oglState.CurrentFBOHeight = m_screenHeight;
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}
			else
			{
				m_oglState.CurrentFBO = renderTarget->OpenGL.FBO;
				m_oglState.CurrentFBOHeight = renderTarget->OpenGL.Height;
				glBindFramebuffer(GL_FRAMEBUFFER, renderTarget->OpenGL.FBO);
			}
			
		
			return NxnaResult::Success;
		}
		break;
		default:
			return NxnaResult::NotSupported;
		}
	}

	void GraphicsDevice::DestroyRenderTarget(RenderTarget* renderTarget)
	{
		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			if (renderTarget->Direct3D11.DefaultDepthStencilView)
				renderTarget->Direct3D11.DefaultDepthStencilView->Release();
			if (renderTarget->Direct3D11.DefaultDepthStencilTexture)
				renderTarget->Direct3D11.DefaultDepthStencilTexture->Release();

			memset(renderTarget, 0, sizeof(RenderTarget));
		}
		break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			glDeleteFramebuffers(1, &renderTarget->OpenGL.FBO);
			renderTarget->OpenGL.FBO = 0;
		}
		break;
		default:
			;
		}
	}

	NxnaResult GraphicsDevice::CreateRenderTargetColorAttachment(const RenderTargetColorAttachmentDesc* desc, RenderTargetColorAttachment* result)
	{
		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
			ZeroMemory(&rtvDesc, sizeof(rtvDesc));
			rtvDesc.Format = DXGI_FORMAT_UNKNOWN;
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			rtvDesc.Texture2D.MipSlice = 0;

			auto r = m_d3d11State.Device->CreateRenderTargetView(desc->Texture.Direct3D11.m_texture, &rtvDesc, &result->Direct3D11.View);
			if (FAILED(r))
			{
				NXNA_SET_ERROR_DETAILS(r, "CreateRenderTargetView() failed");
				return NxnaResult::UnknownError;
			}

			return NxnaResult::Success;
		}
		break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			result->OpenGL.Texture = desc->Texture;

			return NxnaResult::Success;
		}
		break;
		default:
			return NxnaResult::NotSupported;
		}

		return NxnaResult::NotSupported;
	}

	NxnaResult GraphicsDevice::CreateRenderTargetDepthAttachment(const RenderTargetDepthAttachmentDesc* desc, RenderTargetDepthAttachment* result)
	{
		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
			ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
			depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			depthStencilViewDesc.Texture2D.MipSlice = 0;
			
			auto r = m_d3d11State.Device->CreateDepthStencilView(desc->Texture.Direct3D11.m_texture, &depthStencilViewDesc, &result->Direct3D11.View);
			if (FAILED(r))
			{
				NXNA_SET_ERROR_DETAILS(r, "CreateDepthStencilView() failed");
				return NxnaResult::UnknownError;
			}

			return NxnaResult::Success;
		}
		break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			result->OpenGL.Texture = desc->Texture;

			return NxnaResult::Success;
		}
			break;
		default:
			return NxnaResult::NotSupported;
		}

		return NxnaResult::NotSupported;
	}

	NxnaResult GraphicsDevice::SetRenderTargetAttachments(RenderTarget* renderTarget, RenderTargetColorAttachment** colorAttachments, unsigned int numColorAttachments, RenderTargetDepthAttachment* depthAttachment)
	{
		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			memset(renderTarget->Direct3D11.ColorAttachments, 0, sizeof(renderTarget->Direct3D11.ColorAttachments));

			if (numColorAttachments > 8) numColorAttachments = 8;
			for (unsigned int i = 0; i < numColorAttachments; i++)
				renderTarget->Direct3D11.ColorAttachments[i] = colorAttachments[i]->Direct3D11.View;

			if (depthAttachment == nullptr)
				renderTarget->Direct3D11.DepthAttachment = renderTarget->Direct3D11.DefaultDepthStencilView;

			return NxnaResult::Success;
		}
		break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			if (!glNamedFramebufferTexture)
				glBindFramebuffer(GL_FRAMEBUFFER, renderTarget->OpenGL.FBO);

			// make sure we don't set too many render target attachments
			if (numColorAttachments > m_caps.MaxRenderTargets)
				numColorAttachments = m_caps.MaxRenderTargets;

			for (unsigned int i = 0; i < numColorAttachments; i++)
			{
				GLuint texHandle = 0;
				if (colorAttachments[i] != nullptr)
					texHandle = colorAttachments[i]->OpenGL.Texture.OpenGL.Handle;

				if (glNamedFramebufferTexture)
					glNamedFramebufferTexture(renderTarget->OpenGL.FBO, GL_COLOR_ATTACHMENT0 + i, texHandle, 0);
				else
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texHandle, 0);
			}

			// continue until GL_MAX_COLOR_ATTACHMENTS and un-attach everything
			for (unsigned int i = numColorAttachments; i < m_caps.MaxRenderTargets; i++)
			{
				if (glNamedFramebufferTexture)
					glNamedFramebufferTexture(renderTarget->OpenGL.FBO, GL_COLOR_ATTACHMENT0 + i, 0, 0);
				else
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, 0, 0);
			}

			if (depthAttachment)
			{
				if (glNamedFramebufferRenderbuffer)
					glNamedFramebufferRenderbuffer(renderTarget->OpenGL.FBO, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthAttachment->OpenGL.Texture.OpenGL.Handle);
				else
					glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthAttachment->OpenGL.Texture.OpenGL.Handle);
			}
			else
			{
				if (glNamedFramebufferRenderbuffer)
					glNamedFramebufferRenderbuffer(renderTarget->OpenGL.FBO, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderTarget->OpenGL.DefaultDepthStencilTexture);
				else
					glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderTarget->OpenGL.DefaultDepthStencilTexture);
			}

			if (glNamedFramebufferRenderbuffer)
			{
				GLenum status = glCheckNamedFramebufferStatus(renderTarget->OpenGL.FBO, GL_FRAMEBUFFER);
				if (status != GL_FRAMEBUFFER_COMPLETE)
					return NxnaResult::UnknownError;
			}
			else
			{
				GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
				if (status != GL_FRAMEBUFFER_COMPLETE)
					return NxnaResult::UnknownError;
			}

			if (!glNamedFramebufferTexture)
				glBindFramebuffer(GL_FRAMEBUFFER, m_oglState.CurrentFBO);

			return NxnaResult::Success;
		}
		break;
		default:
			return NxnaResult::NotSupported;
		}

		return NxnaResult::NotSupported;
	}

	void GraphicsDevice::DestroyRenderTargetColorAttachment(RenderTargetColorAttachment* attachment)
	{
		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			attachment->Direct3D11.View->Release();
			attachment->Direct3D11.View = nullptr;
		}
		break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			// nothing
		}
			break;
		default:
			;
		}
	}

	void GraphicsDevice::DestroyRenderTargetDepthAttachment(RenderTargetDepthAttachment* attachment)
	{
		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			attachment->Direct3D11.View->Release();
			attachment->Direct3D11.View = nullptr;
		}
		break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			// nothing
		}
		break;
		default:
			;
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
		default:
			return nullptr;
		}
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
		default:
			return nullptr;
		}
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
		default:
			return nullptr;
		}
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
			break;
		default:
			;
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
			break;
		default:
			;
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
			break;
		default:
			;
		}
	}

	void GraphicsDevice::DrawIndexed(PrimitiveType primitiveType, int baseVertex, int minVertexIndex, int numVertices, int startIndex, int indexCount)
	{
		applyDirtyStates();

		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			switch (primitiveType)
			{
			case PrimitiveType::TriangleList:
				m_d3d11State.Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				break;
			case PrimitiveType::TriangleStrip:
				m_d3d11State.Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
				break;
			case PrimitiveType::LineList:
				m_d3d11State.Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
				break;
			case PrimitiveType::LineStrip:
				m_d3d11State.Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
				break;
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
			switch (primitiveType)
			{
			case PrimitiveType::TriangleStrip:
				glPrimitiveType = GL_TRIANGLE_STRIP;
				break;
			case PrimitiveType::TriangleList:
				glPrimitiveType = GL_TRIANGLES;
				break;
			case PrimitiveType::LineList:
				glPrimitiveType = GL_LINES;
				break;
			case PrimitiveType::LineStrip:
				glPrimitiveType = GL_LINE_STRIP;
				break;
			}

			glDrawElementsBaseVertex(glPrimitiveType, indexCount, size, (void*)(intptr_t)(startIndex * (int)m_indices.ElementSize), baseVertex);
		}
			break;
		default:
			;
		}
	}

	void GraphicsDevice::Draw(PrimitiveType primitiveType, int startIndex, int indexCount)
	{
		applyDirtyStates();

		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			switch (primitiveType)
			{
			case PrimitiveType::TriangleList:
				m_d3d11State.Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				break;
			case PrimitiveType::TriangleStrip:
				m_d3d11State.Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
				break;
			case PrimitiveType::LineList:
				m_d3d11State.Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
				break;
			case PrimitiveType::LineStrip:
				m_d3d11State.Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
				break;
			}

			// TODO
			//m_d3d11State.Context->DrawIndexed(indexCount, startIndex, baseVertex);
		}
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			GLenum glPrimitiveType;
			switch (primitiveType)
			{
			case PrimitiveType::TriangleStrip:
				glPrimitiveType = GL_TRIANGLE_STRIP;
				break;
			case PrimitiveType::TriangleList:
				glPrimitiveType = GL_TRIANGLES;
				break;
			case PrimitiveType::LineList:
				glPrimitiveType = GL_LINES;
				break;
			case PrimitiveType::LineStrip:
				glPrimitiveType = GL_LINE_STRIP;
				break;
			}

			glDrawArrays(glPrimitiveType, startIndex, indexCount);
		}
			break;
		default:
			;
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
			for (unsigned int i = 0; i < 8; i++)
			{
				if (m_d3d11State.CurrentRenderTargetViews[i])
					m_d3d11State.Context->ClearRenderTargetView(m_d3d11State.CurrentRenderTargetViews[i], rgba);
			}
			
		}
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			glClearBufferfv(GL_COLOR, 0, rgba);
		}
			break;
		default:
			;
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
			for (unsigned int i = 0; i < 8; i++)
			{
				if (m_d3d11State.CurrentRenderTargetViews[i])
					m_d3d11State.Context->ClearRenderTargetView(m_d3d11State.CurrentRenderTargetViews[i], rgba);
			}
		}
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			glClearBufferfv(GL_COLOR, 0, rgba);
		}
			break;
		default:
			;
		}
	}
	
	void GraphicsDevice::ClearColor(const float* colorRGBA4f)
	{
		switch (GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			for (unsigned int i = 0; i < 8; i++)
			{
				if (m_d3d11State.CurrentRenderTargetViews[i])
					m_d3d11State.Context->ClearRenderTargetView(m_d3d11State.CurrentRenderTargetViews[i], colorRGBA4f);
			}
		}
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			glClearBufferfv(GL_COLOR, 0, colorRGBA4f);
		}
			break;
		default:
			;
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
			m_d3d11State.Context->ClearDepthStencilView(m_d3d11State.CurrentDepthStencilView, flags , depthValue, (UINT8)stencilValue);
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
		default:
			;
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
		default:
			;
		}
	}

	void GraphicsDevice::applyDirtyStates()
	{
		switch (GetType())
		{
		case GraphicsDeviceType::OpenGl41:
		{
			// setup the VAO
			{
				if (m_oglState.CurrentVertexBuffersDirty ||
					m_oglState.CurrentIndexBufferDirty ||
					m_oglState.CurrentInputElementsDirty)
				{
					glBindVertexArray(m_oglState.DefaultVAO);
					

					if (m_oglState.CurrentIndexBufferDirty)
						glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_oglState.CurrentIndexBuffer);

					
					if (m_oglState.CurrentVertexBuffersDirty ||
						m_oglState.CurrentInputElementsDirty)
					{
						unsigned int currentBuffer = 0;

						for (unsigned int i = 0; i < m_shaderPipeline.OpenGL.NumElements; i++)
						{
							int sizeOfElement = 0;
							GLenum type;
							GLboolean normalize;

							auto format = m_shaderPipeline.OpenGL.VertexElements[i].ElementFormat;
							if (format == InputElementFormat::Color)
							{
								sizeOfElement = 4;
								type = GL_UNSIGNED_BYTE;
								normalize = GL_TRUE;
							}
							else if (format == InputElementFormat::Byte4)
							{
								sizeOfElement = 4;
								type = GL_UNSIGNED_BYTE;
								normalize = GL_FALSE;
							}
							else
							{
								sizeOfElement = (int)format;
								type = GL_FLOAT;
								normalize = GL_FALSE;
							}

							auto inputSlot = m_shaderPipeline.OpenGL.VertexElements[i].InputSlot;
							if (inputSlot > 32 || m_oglState.CurrentVertexBufferActive[inputSlot] == false)
							{
								glDisableVertexAttribArray(i);
								continue;
							}
							
							if (m_oglState.CurrentVertexBuffers[inputSlot].Buffer != currentBuffer)
							{
								glBindBuffer(GL_ARRAY_BUFFER, m_oglState.CurrentVertexBuffers[inputSlot].Buffer);
								currentBuffer = m_oglState.CurrentVertexBuffers[inputSlot].Buffer;
							}

							glEnableVertexAttribArray(i);
							glVertexAttribPointer(i, sizeOfElement, type, normalize, m_oglState.CurrentVertexBuffers[inputSlot].Stride, (void*)(intptr_t)(m_oglState.CurrentVertexBuffers[inputSlot].Offset + m_shaderPipeline.OpenGL.VertexElements[i].Offset));
						}
					}

					m_oglState.CurrentVertexBuffersDirty = false;
					m_oglState.CurrentIndexBufferDirty = false;
					m_oglState.CurrentInputElementsDirty = false;
					memset(m_oglState.CurrentVertexBufferDirty, 0, sizeof(bool) * 32);
				}
			}
		}
			break;
		default:
			;
		}
	}
}
}
