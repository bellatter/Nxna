#ifndef GRAPHICS_EFFECT_H
#define GRAPHICS_EFFECT_H

#include <cassert>
#include <string>
#include <map>
#include <vector>
#include <cstring>
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

		int* GetRawValue(EffectParameter* parameter);
	};

	class EffectParameter
	{
		friend class Effect;

		Effect* m_parent;
		EffectParameterType m_type;
		int m_numElements;
		void* m_handle;

		int m_value[16];
		Texture2D* m_textureValue;

	public:

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

		std::string Name;

		EffectParameterType GetType() { return m_type; }
		int GetNumElements() { return m_numElements; }
		
		void SetValue(Texture2D* texture)
		{
			m_textureValue = texture;
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