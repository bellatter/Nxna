#include <cassert>
#include "OpenGL.h"
#include "../VertexDeclaration.h"
#include "../GraphicsDeviceCapabilities.h"
#include "OpenGLDevice.h"
#include "GlslBasicEffect.h"
#include "GlslSpriteEffect.h"
#include "GlslDualTextureEffect.h"
#include "GlslAlphaTestEffect.h"
#include "GlTexture2D.h"
#include "GlRenderTarget2D.h"
#include "GlVertexBuffer.h"
#include "GlIndexBuffer.h"

namespace Nxna
{
namespace Graphics
{
namespace OpenGl
{
	OpenGlDevice::OpenGlDevice()
	{
		m_instance = this;
		m_vertexPointersNeedSetup = true;
		m_declaration = nullptr;
		m_effect = nullptr;
		m_caps = new GraphicsDeviceCapabilities();
	}

#ifndef USING_OPENGLES
// FIXME: It looks like the next version of GLEW (1.10.1) will fix this. But for now we have to do it ourselves.
#define GLEWAPIENTRY __stdcall

	void GLEWAPIENTRY errorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, void* param)
	{
		printf("OpenGL Error: %s\n", message);
	}
#endif

	void OpenGlDevice::OnContextCreated()
	{
#ifndef USING_OPENGLES
		glewInit();
		glGetError();

		if (glGenVertexArrays != nullptr)
		{
			GLuint vao;
		//	glGenVertexArrays(1, &vao);
		//	glBindVertexArray(vao);
		}

		if (GLEW_EXT_texture_compression_s3tc)
		{
			//m_caps->SupportsS3tcTextureCompression = true;
		}

		// does the GPU offer Hi-Def level support for non-power-of-2 textures?
		if (GLEW_ARB_texture_non_power_of_two)
		{
			m_caps->SupportsFullNonPowerOfTwoTextures = true;
		}

		if (GLEW_ARB_debug_output)
		{
			glDebugMessageCallbackARB(errorCallback, nullptr);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);

			GLuint unusedIds;
			glDebugMessageControlARB(GL_DONT_CARE,
				GL_DONT_CARE,
				GL_DONT_CARE,
				0,
				&unusedIds,
				true);
		}

#endif

		glEnable(GL_BLEND);

		setClearColor(Color::CornflowerBlue);
		setClearDepth(1.0f);

		GlException::ThrowIfError(__FILE__, __LINE__);

#ifndef USING_OPENGLES
        const GLubyte* version = glGetString(GL_VERSION);
		m_version = (version[0] - '0') * 100 + (version[2] - '0') * 10;

		const GLubyte* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);

		if (glslVersion == nullptr)
		{
			m_caps->SupportsShaders = false;
			m_glslVersion = 0;
		}
		else
		{
			m_caps->SupportsShaders = true;
			m_glslVersion = (glslVersion[0] - '0') * 100 + (glslVersion[2] - '0') * 10;
		}
#else
        m_version = 200;
        m_glslVersion = 100;

		m_caps->SupportsShaders = true;
#endif
	}

	void OpenGlDevice::UpdatePresentationParameters(const PresentationParameters& pp)
	{
		m_presentationParameters = pp;
	}

	CullMode OpenGlDevice::GetRasterizerState()
	{
		if (glIsEnabled(GL_CULL_FACE) == GL_FALSE)
            return CullMode::None;

        int param;
        glGetIntegerv(GL_FRONT_FACE, &param);

        // OpenGL stores what NOT to cull, so we need to reverse it
        if (param == GL_CW)
            return CullMode::CullCounterClockwiseFace;
        else
            return CullMode::CullClockwiseFace;
	}

	void OpenGlDevice::SetRasterizerState(const RasterizerState* state)
	{
		assert(state != nullptr);

		if (state->TheCullMode == CullMode::None)
            glDisable(GL_CULL_FACE);
		else
		{
			glEnable(GL_CULL_FACE);

            if (state->TheCullMode == CullMode::CullClockwiseFace)
                glFrontFace(GL_CCW);
            else
                glFrontFace(GL_CW);
        }
	}

	DepthStencilState OpenGlDevice::GetDepthStencilState()
	{
		DepthStencilState result;

		int func;
		glGetIntegerv(GL_DEPTH_FUNC, &func);
		result.DepthBufferFunction = convertCompareFunction(func);
		result.DepthBufferEnable = glIsEnabled(GL_DEPTH_TEST) > 0;

		int mask;
		glGetIntegerv(GL_DEPTH_WRITEMASK, &mask);
		result.DepthBufferWriteEnable = mask > 0;

		result.StencilEnable = glIsEnabled(GL_STENCIL_TEST) > 0;

		int ref;
		glGetIntegerv(GL_STENCIL_REF, &ref);
		result.ReferenceStencil = ref;

		glGetIntegerv(GL_STENCIL_FUNC, &func);
		result.StencilFunction = convertCompareFunction(func);

		int pass;
		glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS, &pass);
		result.StencilPass = convertStencilOperation(pass);

		return result;
	}

	void OpenGlDevice::SetDepthStencilState(const DepthStencilState* state)
	{
		// depth buffer stuff
		if (state->DepthBufferEnable)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);

		glDepthFunc(convertCompareFunction(state->DepthBufferFunction));
		glDepthMask(state->DepthBufferWriteEnable ? GL_TRUE : GL_FALSE);
		
		// stencil buffer stuff
		if (state->StencilEnable)
			glEnable(GL_STENCIL_TEST);
		else
			glDisable(GL_STENCIL_TEST);

		glStencilFunc(convertCompareFunction(state->StencilFunction), state->ReferenceStencil, 0xffffffff);

		glStencilOp(convertStencilOperation(state->StencilFail), convertStencilOperation(state->StencilDepthBufferFail), convertStencilOperation(state->StencilPass));
	}

	void OpenGlDevice::SetIndices(const IndexBuffer* indices)
	{
		m_indices = static_cast<const GlIndexBuffer*>(indices->GetPimpl());
		m_indices->Bind();
	}

	void OpenGlDevice::Clear(const Color& c)
	{
		if (c != m_clearColor)
		{
			setClearColor(c);
		}

		int mask;
		glGetIntegerv(GL_DEPTH_WRITEMASK, &mask);
		glDepthMask(GL_TRUE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glDepthMask((GLboolean)mask);

        GlException::ThrowIfError(__FILE__, __LINE__);
	}

	void OpenGlDevice::Clear(ClearOptions options, const Color& c, float depth, int stencil)
	{
		if (c != m_clearColor)
			setClearColor(c);

		if (depth != m_clearDepth)
			setClearDepth(depth);

		if (stencil != m_clearStencil)
			setClearStencil(stencil);

		int depthMask = 0;
		int gloptions = 0;
		if ((options & ClearOptions::DepthBuffer) == ClearOptions::DepthBuffer)
		{
			gloptions |= GL_DEPTH_BUFFER_BIT;

			// OpenGL seems to need depth buffer writing enabled for glClear() to have an effect
			glGetIntegerv(GL_DEPTH_WRITEMASK, &depthMask);
			glDepthMask(GL_TRUE);
		}
		if ((options & ClearOptions::Stencil) == ClearOptions::Stencil)
			gloptions |= GL_STENCIL_BUFFER_BIT;
		if ((options & ClearOptions::Target) == ClearOptions::Target)
			gloptions |= GL_COLOR_BUFFER_BIT;

		if (gloptions != 0)
			glClear(gloptions);

		if ((options & ClearOptions::DepthBuffer) == ClearOptions::DepthBuffer)
		{
			// restore the original depth mask state
			glDepthMask((GLboolean)depthMask);
		}
	}

	Viewport OpenGlDevice::GetViewport()
	{
		return m_viewport;
	}

	void OpenGlDevice::SetViewport(const Viewport& viewport)
	{
		m_viewport = viewport;

		// OpenGL stores the bottom-left corner, but XNA
		// stores the upper-left corner, so we have to convert.
		int y = m_presentationParameters.BackBufferHeight - (viewport.Height + viewport.Y);

		glViewport(viewport.X, y, viewport.Width, viewport.Height);
        
        GlException::ThrowIfError(__FILE__, __LINE__);
	}

	void OpenGlDevice::DrawIndexedPrimitives(PrimitiveType primitiveType, int baseVertex, int minVertexIndex, int numVertices, int startIndex, int primitiveCount)
	{
		assert(m_indices != nullptr);
		assert(startIndex + primitiveCount < m_indices->GetIndexCount());

#ifndef NDEBUG
		for (int i = startIndex; i < primitiveCount + startIndex; i++)
		{
			int index = m_indices->GetIndex(i);

			assert(index < m_vertices->GetVertexCount());
            assert(index >= 0);
		}
#endif

		applyDirtyStates();

		IndexElementSize elementSize = m_indices->GetElementSize();

		int size = GL_UNSIGNED_SHORT;
		if (elementSize == IndexElementSize::ThirtyTwoBits)
			size = GL_UNSIGNED_INT;

		GLenum glPrimitiveType;
		if (primitiveType == PrimitiveType::TriangleStrip)
			glPrimitiveType = GL_TRIANGLE_STRIP;
		else
			glPrimitiveType = GL_TRIANGLES;

		glDrawElements(glPrimitiveType, primitiveCount * 3, size, (void*)(startIndex * (int)elementSize));

		GlException::ThrowIfError(__FILE__, __LINE__);
	}

	void OpenGlDevice::DrawPrimitives(PrimitiveType primitiveType, int startVertex, int primitiveCount) {}
	void OpenGlDevice::DrawUserIndexedPrimitives(PrimitiveType primitiveType, void* data, int numVertices, int* indices, int primitiveCount, const VertexDeclaration* vertexDeclaration) {}
	void OpenGlDevice::DrawUserIndexedPrimitives(PrimitiveType primitiveType, void* data, int numVertices, short* indices, int primitiveCount, const VertexDeclaration* vertexDeclaration)
	{
#ifndef NDEBUG
		for (int i = 0; i < primitiveCount * 3; i++)
		{
			int index = indices[i];
            
			assert(index < numVertices);
            assert(index >= 0);
		}
#endif
        
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		GlException::ThrowIfError(__FILE__, __LINE__);

		m_indices = nullptr;
		m_vertices = nullptr;
		m_declaration = vertexDeclaration;
		setupVertexBufferPointers(data);
		SetSamplers();

		GLenum glPrimitiveType;
		if (primitiveType == PrimitiveType::TriangleStrip)
			glPrimitiveType = GL_TRIANGLE_STRIP;
		else
			glPrimitiveType = GL_TRIANGLES;

		glDrawElements(glPrimitiveType, primitiveCount * 3, GL_UNSIGNED_SHORT, indices);
        
        GlException::ThrowIfError(__FILE__, __LINE__);
	}

	void OpenGlDevice::DrawUserPrimitives(PrimitiveType primitiveType, void* data, int primitiveCount, const VertexDeclaration* vertexDeclaration) {}

	void OpenGlDevice::SetVertexBuffer(const VertexBuffer* vertexBuffer)
	{
		if (vertexBuffer != nullptr)
		{
			static_cast<const GlVertexBuffer*>(const_cast<VertexBuffer*>(vertexBuffer)->GetPimpl())->Bind();

			m_vertices = vertexBuffer;
			m_declaration = vertexBuffer->GetDeclaration();
		}
		else
		{
			m_vertices = nullptr;
			m_declaration = nullptr;
		}

		m_vertexPointersNeedSetup = true;
	}

	void OpenGlDevice::SetBlendState(const BlendState* blendState)
	{
		assert(blendState != nullptr);
        GlException::ThrowIfError(__FILE__, __LINE__); 
        
		int colorSrc = convertBlendMode(blendState->ColorSourceBlend);
        int colorDest = convertBlendMode(blendState->ColorDestinationBlend);
        int alphaSrc = convertBlendMode(blendState->AlphaSourceBlend);
        int alphaDest = convertBlendMode(blendState->AlphaDestinationBlend);

        glBlendFuncSeparate(colorSrc, colorDest, alphaSrc, alphaDest);
        
        GlException::ThrowIfError(__FILE__, __LINE__);
        
        glBlendEquationSeparate(convertBlendFunc(blendState->ColorBlendFunction),
            convertBlendFunc(blendState->AlphaBlendFunction));

		GlException::ThrowIfError(__FILE__, __LINE__);
	}

	void OpenGlDevice::SetRenderTarget(RenderTarget2D* renderTarget)
	{
		if (renderTarget == nullptr)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		else
		{
			GlRenderTarget2D* target = static_cast<GlRenderTarget2D*>(renderTarget->GetPimpl());

			glBindFramebuffer(GL_FRAMEBUFFER, target->GetFBO());
		}
	}

	void OpenGlDevice::Present() {}

	void OpenGlDevice::GetBackBufferData(void* data)
	{
#ifndef USING_OPENGLES
		glReadBuffer(GL_FRONT);
#endif
 
		glReadPixels(0, 0, m_presentationParameters.BackBufferWidth, m_presentationParameters.BackBufferHeight, GL_RGB, GL_UNSIGNED_BYTE, data);
	}

	const char* OpenGlDevice::GetVendor()
	{
		return (char*)glGetString(GL_VENDOR);
	}

	const char* OpenGlDevice::GetRenderer()
	{
		return (char*)glGetString(GL_RENDERER);
	}

	void OpenGlDevice::SetSamplers()
	{
		m_effect->ApplySamplerStates(&m_samplers);
	}

	Pvt::ITexture2DPimpl* OpenGlDevice::CreateTexture2DPimpl(int width, int height, bool mipMap, SurfaceFormat format)
	{
		return new GlTexture2D(this, width, height, format);
	}

	Pvt::IRenderTarget2DPimpl* OpenGlDevice::CreateRenderTarget2DPimpl(RenderTarget2D* parentRenderTarget, int width, int height, SurfaceFormat preferredFormat, DepthFormat preferredDepthFormat, int preferredMultiSampleCount, RenderTargetUsage usage)
	{
		GlTexture2D* glTexture = static_cast<GlTexture2D*>(static_cast<Texture2D*>(parentRenderTarget)->GetPimpl());

		return new GlRenderTarget2D(this, glTexture, width, height, preferredFormat, preferredDepthFormat, preferredMultiSampleCount, usage);
	}

	Pvt::IIndexBufferPimpl* OpenGlDevice::CreateIndexBufferPimpl(IndexElementSize size)
	{
		return new GlIndexBuffer(size);
	}

	Pvt::IVertexBufferPimpl* OpenGlDevice::CreateVertexBufferPimpl(bool dynamic, const VertexDeclaration* vertexDeclaration, int vertexCount, BufferUsage usage)
	{
		return new GlVertexBuffer(dynamic, this, vertexDeclaration, vertexCount, usage);
	}

	Pvt::IEffectPimpl* OpenGlDevice::CreateEffectPimpl(Effect* parent)
	{
		return new GlslEffect(this, parent);
	}

	Pvt::BasicEffectPimpl* OpenGlDevice::CreateBasicEffectPimpl(BasicEffect* effect, Pvt::IEffectPimpl* pimpl)
	{
		return new GlslBasicEffect(this, static_cast<GlslEffect*>(pimpl));
	}

	Pvt::SpriteEffectPimpl* OpenGlDevice::CreateSpriteEffectPimpl(SpriteEffect* effect, Pvt::IEffectPimpl* pimpl)
	{
		return new GlslSpriteEffect(this, static_cast<GlslEffect*>(pimpl));
	}

	Pvt::DualTextureEffectPimpl* OpenGlDevice::CreateDualTextureEffectPimpl(DualTextureEffect* effect, Pvt::IEffectPimpl* pimpl)
	{
		return new GlslDualTextureEffect(this, static_cast<GlslEffect*>(pimpl));
	}

	Pvt::AlphaTestEffectPimpl* OpenGlDevice::CreateAlphaTestEffectPimpl(AlphaTestEffect* effect, Pvt::IEffectPimpl* pimpl)
	{
		return new GlslAlphaTestEffect(this, static_cast<GlslEffect*>(pimpl));
	}

	void OpenGlDevice::setClearColor(const Color& c)
	{
		m_clearColor = c;
		glClearColor(m_clearColor.R / 255.0f, m_clearColor.G / 255.0f, m_clearColor.B / 255.0f, m_clearColor.A / 255.0f);
	}

	void OpenGlDevice::setClearDepth(float depth)
	{
		m_clearDepth = depth;
#ifdef USING_OPENGLES
		glClearDepthf(depth);
#else
		glClearDepth(depth);
#endif
	}

	void OpenGlDevice::setClearStencil(int stencil)
	{
		m_clearStencil = stencil;
		glClearStencil(stencil);
	}

	void OpenGlDevice::applyDirtyStates()
	{
		if (m_vertexPointersNeedSetup)
			setupVertexBufferPointers(nullptr);

		SetSamplers();
	}

	void OpenGlDevice::setupVertexBufferPointers(void* verts)
	{
		assert(m_declaration != nullptr);
		assert(m_effect != nullptr);


		for (int i = 0; i < m_declaration->GetNumElements(); i++)
		{
			const GlslAttribute* attrib = m_effect->GetAttribute(m_declaration->GetElements()[i].ElementUsage, m_declaration->GetElements()[i].UsageIndex);
			if (attrib == nullptr)
				continue;

			glEnableVertexAttribArray(attrib->GlHandle);

			int sizeOfElement = 0;
			GLenum type;
			GLboolean normalize;

			VertexElementFormat format = m_declaration->GetElements()[i].ElementFormat;
			if (format == VertexElementFormat::Color)
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

			glVertexAttribPointer(attrib->GlHandle, sizeOfElement, type, normalize, 
				m_declaration->GetStride(), (byte*)verts + m_declaration->GetElements()[i].Offset);
		}

		m_vertexPointersNeedSetup = false;

		GlException::ThrowIfError(__FILE__, __LINE__);
	}

	int OpenGlDevice::convertCompareFunction(CompareFunction func)
	{
		if (func == CompareFunction::Never)
			return GL_NEVER;
		if (func == CompareFunction::Less)
			return GL_LESS;
		if (func == CompareFunction::Equal)
			return GL_EQUAL;
		if (func == CompareFunction::LessEqual)
			return GL_LEQUAL;
		if (func == CompareFunction::Greater)
			return GL_GREATER;
		if (func == CompareFunction::NotEqual)
			return GL_NOTEQUAL;
		if (func == CompareFunction::GreaterEqual)
			return GL_GEQUAL;
		if (func == CompareFunction::Always)
			return GL_ALWAYS;

		return GL_ALWAYS;
	}

	CompareFunction OpenGlDevice::convertCompareFunction(int func)
	{
		if (func == GL_NEVER)
			return CompareFunction::Never;
		if (func == GL_LESS)
			return CompareFunction::Less;
		if (func == GL_EQUAL)
			return CompareFunction::Equal;
		if (func == GL_LEQUAL)
			return CompareFunction::LessEqual;
		if (func == GL_GREATER)
			return CompareFunction::Greater;
		if (func == GL_NOTEQUAL)
			return CompareFunction::NotEqual;
		if (func == GL_GEQUAL)
			return CompareFunction::GreaterEqual;
		if (func == GL_ALWAYS)
			return CompareFunction::Always;

		return CompareFunction::Always;
	}

	int OpenGlDevice::convertStencilOperation(StencilOperation op)
	{
		if (op == StencilOperation::Keep)
			return GL_KEEP;
		if (op == StencilOperation::Zero)
			return GL_ZERO;
		if (op == StencilOperation::Replace)
			return GL_REPLACE;
		if (op == StencilOperation::IncrementSaturation)
			return GL_INCR;
		if (op == StencilOperation::Increment)
			return GL_INCR_WRAP;
		if (op == StencilOperation::DecrementSaturation)
			return GL_DECR;
		if (op == StencilOperation::Decrement)
			return GL_DECR_WRAP;
		if (op == StencilOperation::Invert)
			return GL_INVERT;

		return GL_KEEP;
	}

	StencilOperation OpenGlDevice::convertStencilOperation(int op)
	{
		if (op == GL_KEEP)
			return StencilOperation::Keep;
		if (op == GL_ZERO)
			return StencilOperation::Zero;
		if (op == GL_REPLACE)
			return StencilOperation::Replace;
		if (op == GL_INCR)
			return StencilOperation::IncrementSaturation;
		if (op == GL_INCR_WRAP)
			return StencilOperation::Increment;
		if (op == GL_DECR)
			return StencilOperation::DecrementSaturation;
		if (op == GL_DECR_WRAP)
			return StencilOperation::Decrement;
		if (op == GL_INVERT)
			return StencilOperation::Invert;

		return StencilOperation::Keep;
	}

	int OpenGlDevice::convertBlendMode(Blend mode)
	{
		if (mode == Blend::One)
            return GL_ONE;
        if (mode == Blend::Zero)
            return GL_ZERO;
        if (mode == Blend::SourceAlpha)
            return GL_SRC_ALPHA;
        if (mode == Blend::DestinationAlpha)
            return GL_DST_ALPHA;
        if (mode == Blend::InverseSourceAlpha)
            return GL_ONE_MINUS_SRC_ALPHA;
        if (mode == Blend::InverseDestinationAlpha)
            return GL_ONE_MINUS_DST_ALPHA;

        return GL_ZERO;
	}

	// these are not available in OpenGL ES
#ifndef GL_MIN
#define GL_MIN 0x8007
#endif
#ifndef GL_MAX
#define GL_MAX 0x8008
#endif

	int OpenGlDevice::convertBlendFunc(BlendFunction func)
	{
		if (func == BlendFunction::Add)
            return GL_FUNC_ADD;
        else if (func == BlendFunction::Subtract)
            return GL_FUNC_SUBTRACT;
        else if (func == BlendFunction::ReverseSubtract)
            return GL_FUNC_REVERSE_SUBTRACT;
        else if (func == BlendFunction::Min)
            return GL_MIN;
        else
            return GL_MAX;
	}

	GlException::GlException(int glError, const char* file, int line)
		: GraphicsException("GlException")
	{
		m_message = "GlException: err " + glError + std::string(" at ") + std::string(file);
	}

#ifndef NXNA_DISABLE_OPENGL_ERRORS
	void GlException::ThrowIfError(const char* file, int line)
	{
		int err = glGetError();
		if (err != 0)
			throw GlException(err);
	}
#endif
}
}
}
