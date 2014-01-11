#ifndef GRAPHICS_EFFECT_H
#define GRAPHICS_EFFECT_H

#include <cassert>
#include <string>
#include <map>
#include <vector>
#include <cstring>
#include "../NxnaConfig.h"
#include "../Matrix.h"
#include "../Vector2.h"
#include "../Vector3.h"
#include "../Vector4.h"

namespace Nxna
{
namespace Graphics
{
	class Texture2D;
	class EffectParameter;

	NXNA_ENUM(EffectParameterType)
		Bool,
		Int32,
		Single,
		String,
		Texture,
		Texture1D,
		Texture2D,
		Texture3D,
		TextureCube,
		Void
	END_NXNA_ENUM(EffectParameterType)

	class GraphicsDevice;

	namespace Pvt
	{
		class IEffectPimpl;
	}

	class Effect
	{
	public:

		virtual void Apply();

		EffectParameter* GetParameter(const char* name);
		EffectParameter* GetParameter(int index);
		int GetNumParameters();
		
		GraphicsDevice* GetGraphicsDevice() { return m_device; }

	protected:

		Effect(GraphicsDevice* device);
		Effect(GraphicsDevice* device, Pvt::IEffectPimpl* pimpl);
		virtual ~Effect() { }

		GraphicsDevice* m_device;
		Pvt::IEffectPimpl* m_pimpl;
	};

	class EffectParameter
	{
		friend class Pvt::IEffectPimpl;

		Effect* m_parent;
		EffectParameterType m_type;
		int m_numElements;
		void* m_handle;

		int m_value[16];
		Texture2D* m_textureValue;

		EffectParameter(Effect* parent, EffectParameterType type, int numElements, void* handle, const char* name)
		{
			m_parent = parent;
			m_type = type;
			m_numElements = numElements;
			m_handle = handle;
			Name = name;

			memset(m_value, 0, sizeof(m_value));
			m_textureValue = nullptr;
		}

	public:

		std::string Name;

		EffectParameterType GetType() { return m_type; }
		int GetNumElements() { return m_numElements; }
		
		void SetValue(Texture2D* texture)
		{
			m_textureValue = texture;
		}

		void SetValue(float value)
		{
			memcpy(m_value, &value, sizeof(float));
		}

		void SetValue(const Vector4& value) 
		{
			memcpy(m_value, &value, sizeof(value));
		}

		void SetValue(float matrix4x4[])
		{
			memcpy(m_value, matrix4x4, sizeof(float) * 16);
		}

		void SetValue(const Matrix& matrix)
		{
			memcpy(m_value, matrix.C, sizeof(float) * 16);
		}

		float GetValueSingle()
		{
			float r;
			memcpy(&r, m_value, sizeof(float));
			return r;
		}
		
		void GetValueSingleArray(float* destination, int count)
		{
			memcpy(destination, m_value, sizeof(float) * count);
		}

		Vector2 GetValueVector2()
		{
			Vector2 v;
			memcpy(&v, m_value, sizeof(float) * 2);
			return v;
		}

		Vector3 GetValueVector3()
		{
			Vector3 v;
			memcpy(&v, m_value, sizeof(float) * 3);
			return v;
		}

		Vector4 GetValueVector4()
		{
			Vector4 v;
			memcpy(&v, m_value, sizeof(float) * 4);
			return v;
		}

		int GetValueInt32() { return m_value[0]; }

		Texture2D* GetValueTexture2D() { return m_textureValue; }

		void* GetHandle() { return m_handle; }
	};
}
}

#endif // GRAPHICS_EFFECT_H