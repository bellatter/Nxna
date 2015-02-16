#ifndef NXNA_AUDIO_OPENAL_OPENALWRAPPER_H
#define NXNA_AUDIO_OPENAL_OPENALWRAPPER_H

#include "../../NxnaConfig.h"

#ifdef NXNA_AUDIOENGINE_OPENAL
#ifdef __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#elif defined NXNA_PLATFORM_NIX
#include <AL/al.h>
#include <AL/alc.h>
#else
#ifdef NXNA_PLATFORM_WIN32
#define AL_NO_PROTOTYPES
#endif
#include <al.h>
#include <alc.h>
#endif
#endif

bool InitOpenAL();

#ifdef NXNA_PLATFORM_WIN32

extern LPALCCREATECONTEXT         nxna_alcCreateContext;
extern LPALCMAKECONTEXTCURRENT    nxna_alcMakeContextCurrent;
extern LPALCPROCESSCONTEXT        nxna_alcProcessContext;
extern LPALCSUSPENDCONTEXT        nxna_alcSuspendContext;
extern LPALCDESTROYCONTEXT        nxna_alcDestroyContext;
extern LPALCGETCURRENTCONTEXT     nxna_alcGetCurrentContext;
extern LPALCGETCONTEXTSDEVICE     nxna_alcGetContextsDevice;
extern LPALCOPENDEVICE            nxna_alcOpenDevice;
extern LPALCCLOSEDEVICE           nxna_alcCloseDevice;
extern LPALCGETERROR              nxna_alcGetError;
extern LPALCISEXTENSIONPRESENT    nxna_alcIsExtensionPresent;
extern LPALCGETPROCADDRESS        nxna_alcGetProcAddress;
extern LPALCGETENUMVALUE          nxna_alcGetEnumValue;
extern LPALCGETSTRING             nxna_alcGetString;
extern LPALCGETINTEGERV           nxna_alcGetIntegerv;
extern LPALCCAPTUREOPENDEVICE     nxna_alcCaptureOpenDevice;
extern LPALCCAPTURECLOSEDEVICE    nxna_alcCaptureCloseDevice;
extern LPALCCAPTURESTART          nxna_alcCaptureStart;
extern LPALCCAPTURESTOP           nxna_alcCaptureStop;
extern LPALCCAPTURESAMPLES        nxna_alcCaptureSamples;

extern LPALENABLE                 alEnable;
extern LPALDISABLE                alDisable;
extern LPALISENABLED              alIsEnabled;
extern LPALGETSTRING              alGetString;
extern LPALGETBOOLEANV            alGetBooleanv;
extern LPALGETINTEGERV            alGetIntegerv;
extern LPALGETFLOATV              alGetFloatv;
extern LPALGETDOUBLEV             alGetDoublev;
extern LPALGETBOOLEAN             alGetBoolean;
extern LPALGETINTEGER             alGetInteger;
extern LPALGETFLOAT               alGetFloat;
extern LPALGETDOUBLE              alGetDouble;
extern LPALGETERROR               alGetError;
extern LPALISEXTENSIONPRESENT     alIsExtensionPresent;
extern LPALGETPROCADDRESS         alGetProcAddress;
extern LPALGETENUMVALUE           alGetEnumValue;
extern LPALLISTENERF              alListenerf;
extern LPALLISTENER3F             alListener3f;
extern LPALLISTENERFV             alListenerfv;
extern LPALLISTENERI              alListeneri;
extern LPALLISTENER3I             alListener3i;
extern LPALLISTENERIV             alListeneriv;
extern LPALGETLISTENERF           alGetListenerf;
extern LPALGETLISTENER3F          alGetListener3f;
extern LPALGETLISTENERFV          alGetListenerfv;
extern LPALGETLISTENERI           alGetListeneri;
extern LPALGETLISTENER3I          alGetListener3i;
extern LPALGETLISTENERIV          alGetListeneriv;
extern LPALGENSOURCES             alGenSources;
extern LPALDELETESOURCES          alDeleteSources;
extern LPALISSOURCE               alIsSource;
extern LPALSOURCEF                alSourcef;
extern LPALSOURCE3F               alSource3f;
extern LPALSOURCEFV               alSourcefv;
extern LPALSOURCEI                alSourcei;
extern LPALSOURCE3I               alSource3i;
extern LPALSOURCEIV               alSourceiv;
extern LPALGETSOURCEF             alGetSourcef;
extern LPALGETSOURCE3F            alGetSource3f;
extern LPALGETSOURCEFV            alGetSourcefv;
extern LPALGETSOURCEI             alGetSourcei;
extern LPALGETSOURCE3I            alGetSource3i;
extern LPALGETSOURCEIV            alGetSourceiv;
extern LPALSOURCEPLAYV            alSourcePlayv;
extern LPALSOURCESTOPV            alSourceStopv;
extern LPALSOURCEREWINDV          alSourceRewindv;
extern LPALSOURCEPAUSEV           alSourcePausev;
extern LPALSOURCEPLAY             alSourcePlay;
extern LPALSOURCESTOP             alSourceStop;
extern LPALSOURCEREWIND           alSourceRewind;
extern LPALSOURCEPAUSE            alSourcePause;
extern LPALSOURCEQUEUEBUFFERS     alSourceQueueBuffers;
extern LPALSOURCEUNQUEUEBUFFERS   alSourceUnqueueBuffers;
extern LPALGENBUFFERS             alGenBuffers;
extern LPALDELETEBUFFERS          alDeleteBuffers;
extern LPALISBUFFER               alIsBuffer;
extern LPALBUFFERDATA             alBufferData;
extern LPALBUFFERF                alBufferf;
extern LPALBUFFER3F               alBuffer3f;
extern LPALBUFFERFV               alBufferfv;
extern LPALBUFFERI                alBufferi;
extern LPALBUFFER3I               alBuffer3i;
extern LPALBUFFERIV               alBufferiv;
extern LPALGETBUFFERF             alGetBufferf;
extern LPALGETBUFFER3F            alGetBuffer3f;
extern LPALGETBUFFERFV            alGetBufferfv;
extern LPALGETBUFFERI             alGetBufferi;
extern LPALGETBUFFER3I            alGetBuffer3i;
extern LPALGETBUFFERIV            alGetBufferiv;
extern LPALDOPPLERFACTOR          alDopplerFactor;
extern LPALDOPPLERVELOCITY        alDopplerVelocity;
extern LPALSPEEDOFSOUND           alSpeedOfSound;
extern LPALDISTANCEMODEL          alDistanceModel;

#else
#define nxna_alcCreateContext alcCreateContext;
#define nxna_alcMakeContextCurrent alcMakeContextCurrent;
#define nxna_alcProcessContext alcProcessContext;
#define nxna_alcSuspendContext alcSuspendContext;
#define nxna_alcDestroyContext alcDestroyContext;
#define nxna_alcGetCurrentContext alcGetCurrentContext;
#define nxna_alcGetContextsDevice alcGetContextsDevice;
#define nxna_alcOpenDevice alcOpenDevice;
#define nxna_alcCloseDevice alcCloseDevice;
#define nxna_alcGetError alcGetError;
#define nxna_alcIsExtensionPresent alcIsExtensionPresent;
#define nxna_alcGetProcAddress alcGetProcAddress;
#define nxna_alcGetEnumValue alcGetEnumValue;
#define nxna_alcGetString alcGetString;
#define nxna_alcGetIntegerv alcGetIntegerv;
#define nxna_alcCaptureOpenDevice alcCaptureOpenDevice;
#define nxna_alcCaptureCloseDevice alcCaptureCloseDevice;
#define nxna_alcCaptureStart alcCaptureStart;
#define nxna_alcCaptureStop alcCaptureStop;
#define nxna_alcCaptureSamples alcCaptureStop;
#endif


#endif // NXNA_AUDIO_OPENAL_OPENALWRAPPER_H

