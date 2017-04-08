#ifndef NXNA_GRAPHICS_PIPELINESTATE_H
#define NXNA_GRAPHICS_PIPELINESTATE_H

#include "../NxnaCommon.h"

namespace Nxna
{
namespace Graphics
{
	enum class Blend : nxna_byte
	{
		Zero,
		One,
		SourceAlpha,
		InverseSourceAlpha,
		DestinationAlpha,
		InverseDestinationAlpha
	};

	enum class BlendFunction : nxna_byte
	{
		/// Add sources 1 and 2
		Add,

		/// Use the maximum of sources 1 and 2
		Max,

		/// Use the minimum of sources 1 and 2
		Min,

		/// Subtract source 2 from source 1
		ReverseSubtract,

		/// Subtract source 1 from source 2
		Subtract
	};

	struct RenderTargetBlendStateDesc
	{
		bool BlendingEnabled;

		BlendFunction AlphaBlendFunction;
		Blend AlphaDestinationBlend;
		Blend AlphaSourceBlend;

		BlendFunction ColorBlendFunction;
		Blend ColorDestinationBlend;
		Blend ColorSourceBlend;
	};
	
	struct BlendStateDesc
	{
		bool IndependentBlendEnabled;

		RenderTargetBlendStateDesc RenderTarget[8];
	};

	struct BlendState
	{
		union
		{
			struct
			{
				BlendStateDesc Desc;
			} OpenGL;
#ifdef NXNA_ENABLE_DIRECT3D11
			struct
			{
				ID3D11BlendState* State;
			} Direct3D11;
#endif
		};
	};

	enum class CompareFunction : nxna_byte
	{
		Always,
		Equal,
		Greater,
		GreaterEqual,
		Less,
		LessEqual,
		Never,
		NotEqual
	};

	enum class StencilOperation : nxna_byte
	{
		Decrement,
		DecrementSaturation,
		Increment,
		IncrementSaturation,
		Invert,
		Keep,
		Replace,
		Zero
	};

	struct DepthStencilStateDesc
	{
		CompareFunction DepthBufferFunction;
		bool DepthBufferEnabled;
		bool DepthBufferWriteEnabled;

		bool StencilEnable;
		int ReferenceStencil;
		CompareFunction StencilFunction;
		StencilOperation StencilPass;
		StencilOperation StencilFail;
		StencilOperation StencilDepthBufferFail;
	};

	struct DepthStencilState
	{
		union
		{
			struct
			{
				DepthStencilStateDesc Desc;
			} OpenGL;
#ifdef NXNA_ENABLE_DIRECT3D11
			struct
			{
				ID3D11DepthStencilState* State;
				int ReferenceStencil;
			} Direct3D11;
#endif
		};
	};

	enum class InputElementFormat
	{
		/// A single 32-bit floating point number
		Single = 1,

		/// Two 32-bit floating point numbers
		Vector2,

		/// Three 32-bit floating point numbers
		Vector3,

		/// Four 32-bit floating point numbers
		Vector4,

		/// Four bytes of color info, packed as RGBA
		Color,

		/// Four bytes
		Byte4,
	};

	enum class InputElementUsage
	{
		Position,
		Normal,
		TextureCoordinate,
		Color
	};

	struct InputElement
	{
		unsigned int Offset;
		InputElementFormat ElementFormat;
		InputElementUsage ElementUsage;
		unsigned int UsageIndex;
		unsigned int InputSlot;
	};


	struct Shader
	{
		union
		{
			struct
			{
				unsigned int Handle;
			} OpenGL;
#ifdef NXNA_ENABLE_DIRECT3D11
			struct
			{
				void* Ptr; // D3D11 has different types for VS, PS, etc, so we have to use void*
			} Direct3D11;
#endif
		};
	};

	struct ShaderBytecode
	{
		const void* Bytecode;
		unsigned int BytecodeLength;
	};

	struct ShaderPipelineDesc
	{
		Shader* VertexShader;
		Shader* PixelShader;
		ShaderBytecode VertexShaderBytecode;
		InputElement* VertexElements;
		int NumElements;
	};

	struct ShaderPipeline
	{
		union
		{
			struct
			{
				unsigned int Pipeline;
				unsigned int VertexShader;
				unsigned int FragmentShader;
				InputElement VertexElements[16];
				int NumElements;
			} OpenGL;
#ifdef NXNA_ENABLE_DIRECT3D11
			struct
			{
				void* VertexShader;
				void* PixelShader;
				void* Layout;
			} Direct3D11;
#endif
		};
	};

	enum class PrimitiveTopologyType : nxna_byte
	{
		Undefined,
		Points,
		Lines,
		Triangles,
		Patches
	};

	enum class CullMode : nxna_byte
	{
		None,
		CullFrontFaces,
		CullBackFaces
	};

	enum class FillMode : nxna_byte
	{
		Solid,
		Wireframe
	};

	struct RasterizerStateDesc
	{
		CullMode CullingMode;
		bool FrontCounterClockwise;
		FillMode FillingMode;
		bool ScissorTestEnabled;
	};

	struct RasterizerState
	{
		union
		{
			struct
			{
				RasterizerStateDesc Desc;
			} OpenGL;
#ifdef NXNA_ENABLE_DIRECT3D11
			struct
			{
				ID3D11RasterizerState* State;
			} Direct3D11;
#endif
		};
	};

	struct PipelineStateDesc
	{
		ShaderPipeline* Shaders;
		BlendState* Blending;
		PrimitiveTopologyType Topology;
	};

	class PipelineState
	{
	public:

		union
		{
			struct
			{
				BlendStateDesc Blending;
				ShaderPipeline* Shaders;
			} OpenGL;
#ifdef NXNA_ENABLE_DIRECT3D11
			struct
			{
				ShaderPipeline* Shaders;
				BlendState* Blending;
			} Direct3D11;
#endif
		};
	};

	enum class TextureFilter
	{
		Point = 0x0,
		Linear = 0x15,
		Anisotropic = 0x55,

		LinearMipPoint = 0x14,
		PointMipLinear = 0x1,

		MinLinearMagPointMipLinear = 0x11,
		MinLinearMagPointMipPoint = 0x10,
		MinPointMagLinearMipLinear = 0x5,
		MinPointMagLinearMipPoint = 0x4
	};

	enum class TextureAddressMode
	{
		Clamp,
		Mirror,
		Wrap,
		Border,
		MirrorOnce
	};

	struct SamplerStateDesc
	{
		TextureFilter Filter;
		TextureAddressMode AddressU;
		TextureAddressMode AddressV;
		TextureAddressMode AddressW;
		float MipLODBias;
		unsigned int MaxAnisotropy;
		float BorderColor[4];
		float MinLOD;
		float MaxLOD;
	};

	struct SamplerState
	{
		union
		{
			struct
			{
				unsigned int Handle;
			} OpenGL;
#ifdef NXNA_ENABLE_DIRECT3D11
			struct
			{
				ID3D11SamplerState * State;
			} Direct3D11;
#endif
		};
	};

	void GetAdditiveBlendState(BlendStateDesc* state);
	void GetNonPreMultipliedBlendState(BlendStateDesc* state);
	void GetOpaqueBlendState(BlendStateDesc* state);

#define NXNA_RENDERTARGETBLENDSTATEDESC_ALPHABLEND { \
		true, \
		\
		Nxna::Graphics::BlendFunction::Add, \
		Nxna::Graphics::Blend::InverseSourceAlpha, \
		Nxna::Graphics::Blend::One, \
		\
		Nxna::Graphics::BlendFunction::Add, \
		Nxna::Graphics::Blend::InverseSourceAlpha, \
		Nxna::Graphics::Blend::One \
		}


#define NXNA_RASTERIZERSTATEDESC_CULLNONE { \
		Nxna::Graphics::CullMode::None, \
		true, \
		Nxna::Graphics::FillMode::Solid, \
		false \
		}

#define NXNA_RASTERIZERSTATEDESC_DEFAULT { \
		Nxna::Graphics::CullMode::CullBackFaces, \
		false, \
		Nxna::Graphics::FillMode::Solid, \
		false \
		}

#define NXNA_DEPTHSTENCIL_DEFAULT { \
		Nxna::Graphics::CompareFunction::Less, \
		true, \
		true, \
		false, \
		0, \
		Nxna::Graphics::CompareFunction::Always, \
		Nxna::Graphics::StencilOperation::Keep, \
		Nxna::Graphics::StencilOperation::Keep, \
		Nxna::Graphics::StencilOperation::Keep \
		}

#define NXNA_DEPTHSTENCIL_DEPTHREAD { \
	Nxna::Graphics::CompareFunction::Less, \
		true, \
		false, \
		false, \
		0, \
		Nxna::Graphics::CompareFunction::Always, \
		Nxna::Graphics::StencilOperation::Keep, \
		Nxna::Graphics::StencilOperation::Keep, \
		Nxna::Graphics::StencilOperation::Keep \
		}

#define NXNA_SAMPLERSTATEDESC_LINEARWRAP { \
		Nxna::Graphics::TextureFilter::Linear, \
		Nxna::Graphics::TextureAddressMode::Wrap, \
		Nxna::Graphics::TextureAddressMode::Wrap, \
		Nxna::Graphics::TextureAddressMode::Wrap, \
		0, \
		1, \
			{1.0f, 1.0f, 1.0f, 1.0f}, \
		-10000.0f, \
		10000.0f \
		}

#define NXNA_SAMPLERSTATEDESC_POINTCLAMP { \
		Nxna::Graphics::TextureFilter::Point, \
		Nxna::Graphics::TextureAddressMode::Clamp, \
		Nxna::Graphics::TextureAddressMode::Clamp, \
		Nxna::Graphics::TextureAddressMode::Clamp, \
		0, \
		1, \
		{1.0f, 1.0f, 1.0f, 1.0f}, \
		-10000.0f, \
		10000.0f \
	}

#define NXNA_SAMPLERSTATEDESC_LINEARCLAMP { \
		Nxna::Graphics::TextureFilter::Linear, \
		Nxna::Graphics::TextureAddressMode::Clamp, \
		Nxna::Graphics::TextureAddressMode::Clamp, \
		Nxna::Graphics::TextureAddressMode::Clamp, \
		0, \
		1, \
		{1.0f, 1.0f, 1.0f, 1.0f}, \
		-10000.0f, \
		10000.0f \
	}

#define NXNA_BLENDSTATEDESC_DEFAULT { \
	false, \
	{ \
		false, \
		Nxna::Graphics::BlendFunction::Add, \
		Nxna::Graphics::Blend::Zero, \
		Nxna::Graphics::Blend::One, \
		Nxna::Graphics::BlendFunction::Add, \
		Nxna::Graphics::Blend::Zero, \
		Nxna::Graphics::Blend::One, \
	} \
	}
}
}

#endif // NXNA_GRAPHICS_PIPELINESTATE_H
