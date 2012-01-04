#ifndef AUDIO_SOUNDEFFECT_H
#define AUDIO_SOUNDEFFECT_H

#include "../NxnaConfig.h"
#include "../Content/ContentManager.h"

namespace Nxna
{
namespace Audio
{
	enum SoundState
	{
		SoundState_Paused,
		SoundState_Playing,
		SoundState_Stopped
	};

	class SoundEffect;
	class AudioListener;
	class AudioEmitter;

	class SoundEffectInstance
	{
		friend class SoundEffect;
		unsigned int m_source;

	public:
		~SoundEffectInstance();

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
	};

	class SoundEffect
	{
		friend class SoundEffectInstance;

		unsigned int m_alBuffer;
		bool m_duration;

		static byte* m_workingData;
		static int m_workingDataLength;

	public:

		float GetDuration() { return m_duration; }

		bool Play();
		bool Play(float volume, float pitch, float pan);
		SoundEffectInstance* CreateInstance();

		static SoundEffect* LoadFrom(Content::FileStream* stream);
	};

	

	class SoundEffectLoader : public Content::IContentResourceLoader
	{
	public:
		virtual const char* GetTypeName() override { return typeid(SoundEffect).name(); }
		virtual void* Load(Content::XnbReader* stream) override;
		virtual void Destroy(void* resource) override;
	};
}
}

#endif // AUDIO_SOUNDEFFECT_H