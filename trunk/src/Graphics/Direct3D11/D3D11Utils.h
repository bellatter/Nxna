#ifndef NXNA_GRAPHICS_DIRECT3D11_D3DUTILS_H
#define NXNA_GRAPHICS_DIRECT3D11_D3DUTILS_H

#include <D3D11.h>
#include "../BlendState.h"

namespace Nxna
{
namespace Graphics
{
namespace Direct3D11
{
	class D3D11Utils
	{
	public:
		static D3D11_BLEND ConvertBlendMode(Blend blend)
		{
			if (blend == Blend::Zero)
				return D3D11_BLEND_ZERO;
			if (blend == Blend::One)
				return D3D11_BLEND_ONE;
			if (blend == Blend::SourceAlpha)
				return D3D11_BLEND_SRC_ALPHA;
			if (blend == Blend::InverseSourceAlpha)
				return D3D11_BLEND_INV_SRC_ALPHA;
			if (blend == Blend::DestinationAlpha)
				return D3D11_BLEND_DEST_ALPHA;
			if (blend == Blend::InverseDestinationAlpha)
				return D3D11_BLEND_INV_DEST_ALPHA;

			// we should never get here
			return D3D11_BLEND_ZERO;
		}

		static D3D11_BLEND_OP ConvertBlendFunc(BlendFunction func)
		{
			if (func == BlendFunction::Add)
				return D3D11_BLEND_OP_ADD;
			if (func == BlendFunction::Subtract)
				return D3D11_BLEND_OP_SUBTRACT;
			if (func == BlendFunction::ReverseSubtract)
				return D3D11_BLEND_OP_REV_SUBTRACT;
			if (func == BlendFunction::Max)
				return D3D11_BLEND_OP_MAX;
			if (func == BlendFunction::Min)
				return D3D11_BLEND_OP_MIN;

			// we should never get here
			return D3D11_BLEND_OP_MAX;
		
		}

		static D3D11_COMPARISON_FUNC ConvertComparisonFunc(CompareFunction func)
		{
			if (func == CompareFunction::Always)
				return D3D11_COMPARISON_ALWAYS;
			if (func == CompareFunction::Equal)
				return D3D11_COMPARISON_EQUAL;
			if (func == CompareFunction::Greater)
				return D3D11_COMPARISON_GREATER;
			if (func == CompareFunction::GreaterEqual)
				return D3D11_COMPARISON_GREATER_EQUAL;
			if (func == CompareFunction::Less)
				return D3D11_COMPARISON_LESS;
			if (func == CompareFunction::LessEqual)
				return D3D11_COMPARISON_LESS_EQUAL;
			if (func == CompareFunction::Never)
				return D3D11_COMPARISON_NEVER;
			if (func == CompareFunction::NotEqual)
				return D3D11_COMPARISON_NOT_EQUAL;

			// we should never get here
			return D3D11_COMPARISON_NEVER;
		}

		static D3D11_FILTER ConvertTextureFilter(TextureFilter filter)
		{
			if (filter == TextureFilter::Linear)
				return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			if (filter == TextureFilter::Point)
				return D3D11_FILTER_MIN_MAG_MIP_POINT;
			if (filter == TextureFilter::Anisotropic)
				return D3D11_FILTER_ANISOTROPIC;
			if (filter == TextureFilter::LinearMipPoint)
				return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
			if (filter == TextureFilter::PointMipLinear)
				return D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
			if (filter == TextureFilter::MinLinearMagPointMipLinear)
				return D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
			if (filter == TextureFilter::MinLinearMagPointMipPoint)
				return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
			if (filter == TextureFilter::MinPointMagLinearMipLinear)
				return D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
			if (filter == TextureFilter::MinPointMagLinearMipPoint)
				return D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;

			// we should never get hear
			return D3D11_FILTER_MIN_MAG_MIP_POINT;
		}

		static D3D11_TEXTURE_ADDRESS_MODE ConvertTextureAddressMode(TextureAddressMode mode)
		{
			if (mode == TextureAddressMode::Clamp)
				return D3D11_TEXTURE_ADDRESS_CLAMP;
			if (mode == TextureAddressMode::Mirror)
				return D3D11_TEXTURE_ADDRESS_MIRROR;
			if (mode == TextureAddressMode::Wrap)
				return D3D11_TEXTURE_ADDRESS_WRAP;

			// we should never get here
			return D3D11_TEXTURE_ADDRESS_CLAMP;
		}

		static D3D11_STENCIL_OP ConvertStencilOperation(StencilOperation op)
		{
			switch(op)
			{
			case StencilOperation::Decrement:
				return D3D11_STENCIL_OP_DECR;
			case StencilOperation::DecrementSaturation:
				return D3D11_STENCIL_OP_DECR_SAT;
			case StencilOperation::Increment:
				return D3D11_STENCIL_OP_INCR;
			case StencilOperation::IncrementSaturation:
				return D3D11_STENCIL_OP_INCR_SAT;
			case StencilOperation::Invert:
				return D3D11_STENCIL_OP_INVERT;
			case StencilOperation::Keep:
				return D3D11_STENCIL_OP_KEEP;
			case StencilOperation::Replace:
				return D3D11_STENCIL_OP_REPLACE;
			case StencilOperation::Zero:
				return D3D11_STENCIL_OP_ZERO;
			}

			// we should never get here
			return D3D11_STENCIL_OP_KEEP;
		}
	};
}
}
}

#endif // NXNA_GRAPHICS_DIRECT3D11_D3DUTILS_H
