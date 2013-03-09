#ifndef NXNA_PLATFORM_ANDROID_JNI_H
#define NXNA_PLATFORM_ANDROID_JNI_H

struct JNIEnv;

namespace Nxna
{
namespace Platform
{
namespace Android
{
	class JNI
	{
		static JNIEnv* m_env;

	public:
		static void SetEnv(JNIEnv* env) { m_env = env; }
		static JNIEnv* GetEnv() { return m_env; }
	};
}
}
}

#endif // NXNA_PLATFORM_ANDROID_JNI_H
