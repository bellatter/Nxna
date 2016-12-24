#ifndef NXNA_PLATFORM_ANDROID_JNI_H
#define NXNA_PLATFORM_ANDROID_JNI_H

namespace Nxna
{
namespace Platform
{
namespace Android
{
	class JNI
	{
		static void* m_env;

	public:
		static void SetEnv(void* env) { m_env = env; }
		static void* GetEnv() { return m_env; }
	};
}
}
}

#endif // NXNA_PLATFORM_ANDROID_JNI_H
