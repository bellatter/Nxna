#ifndef AUDIO_SOUNDEFFECT_H
#define AUDIO_SOUNDEFFECT_H

#include "../NxnaConfig.h"
#include "../Content/ContentManager.h"
#include "../Vector3.h"
#include "SoundState.h"
#include <vector>

NXNA_DISABLE_OVERRIDE_WARNING

namespace Nxna
{
namespace Audio
{
	class AudioSource;
	class SoundEffect;
	class AudioListener;
	class AudioEmitter;

	class SoundEffectInstance
	{
		friend class SoundEffect;
		bool m_isFireAndForget;
		SoundEffect* m_parent;
		void* m_bufferHandle;
		bool m_isLooped;
		float m_gain;
		Vector3 m_cachedPosition;
		bool m_positioned;

#ifdef NXNA_AUDIOENGINE_OPENAL
		int m_source;
#endif

	public:
		

		void Play();
		void Stop();
		void Pause();

		bool IsLooped();
		void IsLooped(bool looped);
		float Volume();
		void Volume(float volume);
		SoundState GetState();

		void Apply3D(const AudioListener* listener, const AudioEmitter* emitter);

	private:
		SoundEffectInstance(SoundEffect* effect);
		~SoundEffectInstance();

		void updateParent(SoundEffect* parent);
	};

	class SoundEffect
	{
		friend class SoundEffectInstance;

#ifdef NXNA_AUDIOENGINE_OPENAL
		int m_buffer;
#endif
		float m_duration;

		std::vector<SoundEffectInstance*> m_children;

		static std::vector<byte> m_workingData;
		static std::vector<SoundEffectInstance*> m_instancePool;
		static std::vector<SoundEffectInstance*> m_fireAndForgetInstances;

	public:
		~SoundEffect();
		float GetDuration() { return m_duration; }

		bool Play();
		bool Play(float volume, float pitch, float pan);
		SoundEffectInstance* CreateInstance();
		static void DestroyInstance(SoundEffectInstance* instance);

		static void SetDistanceScale(float scale);
		static void SetMasterVolume(float volume);

		static SoundEffect* LoadFrom(Content::Stream* stream);

	private:
		SoundEffect() {}
	};

	

	class SoundEffectLoader : public Content::IContentReader
	{
	public:
		virtual const char* GetTypeName() override { return typeid(SoundEffect).name(); }
		virtual void* Read(Content::XnbReader* stream) override;
		virtual void Destroy(void* resource) override;
	};
}
}

NXNA_ENABLE_OVERRIDE_WARNING

#endif // AUDIO_SOUNDEFFECT_H