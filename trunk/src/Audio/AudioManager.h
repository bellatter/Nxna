#ifndef NXNA_AUDIO_AUDIOMANAGER_H
#define NXNA_AUDIO_AUDIOMANAGER_H

#include "../NxnaConfig.h"
#include "../Vector3.h"
#include "SoundState.h"

namespace Nxna
{
namespace Audio
{
	struct AudioBuffer
	{
		void* Handle;

		static void Create(int numChannels, int numBitsPerSample, int numSamplesPerSecond, byte* data, int numBytes, AudioBuffer* result);
	};

	class AudioSource
	{
		void* m_handle;
		void* m_bufferHandle;

	public:
		AudioSource();
		~AudioSource();
		bool IsAvailable();

		void SetBuffer(void* buffer);
		void Play(float volume, float pitch, float pan);
		void Pause();
		void Stop();

		void SetGain(float gain);

		SoundState GetState();
		bool IsLooping();
		void IsLooping(bool looping);

		void SetPosition(bool relative, const Vector3& position);
	};

	class AudioListener;
	class AudioEmitter;

	class AudioManager
	{
		friend class AudioSource;

		static void* m_device;
		static void* m_context;
		
		static const int MAX_SOURCES = 15;

		struct SourceInfo
		{
			AudioSource* Source;
			void* Owner;
		};

		static SourceInfo m_sources[MAX_SOURCES];
		static float m_distanceScale;

	public:
		static void Init();
		static void Shutdown();

		static AudioSource* GetFreeSource(void* owner);
		static void ReleaseSource(AudioSource* source);
		static bool IsSourceOwner(AudioSource* source, void* owner);

		static void SetDistanceScale(float scale);
		static void SetMasterVolume(float volume);

		static void Apply3D(AudioSource* source, const AudioListener* listener, const AudioEmitter* emitter);
	};
}
}

#endif // NXNA_AUDIO_AUDIOMANAGER_H