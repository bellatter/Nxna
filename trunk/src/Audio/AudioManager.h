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

#if defined NXNA_AUDIOENGINE_OPENSL
		int NumChannels;
		int NumBitsPerSample;
		int NumSamplesPerSecond;
		byte* Data;
		int DataLength;
#endif

		static void Create(int numChannels, int numBitsPerSample, int numSamplesPerSecond, byte* data, int numBytes, AudioBuffer* result);
	};

	class AudioSource
	{
		void* m_handle;
		void* m_bufferHandle;
		bool m_isLooping;
		Vector3 m_position;
		bool m_positionIsRelative;

#if defined NXNA_AUDIOENGINE_OPENSL
		bool m_playing;
#endif

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
		bool IsLooping() { return m_isLooping; }
		void IsLooping(bool looping);

		void SetPosition(bool relative, const Vector3& position);

#if defined NXNA_AUDIOENGINE_OPENSL
		void OnStop() { m_playing = false; }
#endif
	};

	class AudioListener;
	class AudioEmitter;

	class AudioManager
	{
		friend class AudioSource;

#if defined NXNA_AUDIOENGINE_OPENAL
		static void* m_device;
		static void* m_context;
#elif defined NXNA_AUDIOENGINE_OPENSL
		static void* m_engine;
		static void* m_outputMix;
		static void* m_engineInterface;
#endif
		
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

#if defined NXNA_AUDIOENGINE_OPENSL
		static void* GetOutputMix() { return m_outputMix; }
		static void* GetEngineInterface() { return m_engineInterface; }
#endif
	};
}
}

#endif // NXNA_AUDIO_AUDIOMANAGER_H