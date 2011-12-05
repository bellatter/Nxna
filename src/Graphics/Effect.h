#ifndef GRAPHICS_EFFECT_H
#define GRAPHICS_EFFECT_H

#include <cassert>
#include <string>
#include <map>
#include <vector>
#include <cstring>
#include "../Matrix.h"
#include "../Vector4.h"

namespace Nxna
{
namespace Graphics
{
	class Texture2D;
	class EffectParameter;

	enum EffectParameterType
	{
		EffectParameterType_Bool,
		EffectParameterType_Int32,
		EffectParameterType_Single,
		EffectParameterType_String,
		EffectParameterType_Texture,
		EffectParameterType_Texture1D,
		EffectParameterType_Texture2D,
		EffectParameterType_Texture3D,
		EffectParameterType_TextureCube,
		EffectParameterType_Void
	};

	class GraphicsDevice;

	class Effect
	{
		friend class EffectParameter;

	public:

		virtual void Apply() = 0;

		virtual EffectParameter* GetParameter(const char* name) = 0;
		virtual EffectParameter* GetParameter(int index) = 0;
		virtual int GetNumParameters() = 0;
		
		GraphicsDevice* GetGraphicsDevice() { return m_device; }

	protected:

		virtual ~Effect() { }

		GraphicsDevice* m_device;

		virtual EffectParameter* AddParameter(EffectParameterType type, void* handle, const char* name) = 0;

		virtual void SetParameter(EffectParameter* param, Texture2D* texture) = 0;
		virtual void SetParameter(EffectParameter* param, const Vector4& value) = 0;
		virtual void SetParameter(EffectParameter* param, float matrix4x4[]) = 0;
		virtual void SetParameter(EffectParameter* param, const Matrix& matrix) = 0;

	};

	class EffectParameter
	{
		Effect* m_parent;
		EffectParameterType m_type;
		void* m_handle;

	public:

		EffectParameter(Effect* parent, EffectParameterType type, void* handle, const char* name)
		{
			m_parent = parent;
			m_type = type;
			m_handle = handle;
			Name = name;
		}

		std::string Name;

		EffectParameterType GetType() { return m_type; }
		
		void SetValue(Texture2D* texture) { m_parent->SetParameter(this, texture); }
		void SetValue(const Vector4& value) { m_parent->SetParameter(this, value); }
		void SetValue(float matrix4x4[]) { m_parent->SetParameter(this, matrix4x4); }
		void SetValue(const Matrix& matrix) { m_parent->SetParameter(this, matrix); }

		void* GetHandle() { return m_handle; }
	};
}
}

#endif // GRAPHICS_EFFECT_H