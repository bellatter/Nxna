#ifndef NXNACONFIG_H
#define NXNACONFIG_H

#include "Platform/PlatformDefs.h"

typedef unsigned char byte;

// are we using OpenGL ES?
#if defined NXNA_PLATFORM_APPLE_IOS || defined NXNA_PLATFORM_NACL || defined NXNA_PLATFORM_ANDROID
#define USING_OPENGLES
#endif

// which sound engine should we use?
#if defined NXNA_PLATFORM_ANDROID
#define NXNA_AUDIOENGINE_OPENSL
#else
#define NXNA_AUDIOENGINE_OPENAL
#endif

#if defined NXNA_PLATFORM_APPLE_IOS
#define NXNA_PLATFORMENGINE_IOS
#define NXNA_DISABLE_OGG
#else
#define NXNA_PLATFORMENGINE_SDL
#endif

// disable Visual C++ 2010 specific stuff if needed
#if !defined _MSC_VER || _MSC_VER < 1600
#if __cplusplus < 200103L
#define nullptr 0
#endif
#define override
#endif

// allow typesafe enums
#if !defined DISABLE_NXNA_CLASS_ENUMS
#define NXNA_ENUM(e) enum class e {
#define NXNA_BITWISE_ENUM(e) enum e {
#define END_NXNA_ENUM(e) };
#else
template<typename def, typename inner = typename def::type>
class safe_enum : public def
{
	typedef typename def::type type;
	inner val;
public:
	safe_enum(int v) : val((type)v) {}
	safe_enum(type v) : val(v) {}
	safe_enum() : val((inner)0) {}
	inner underlying() const { return val; }
 
	//bool operator == (const safe_enum & s) const { return this->val == s.val; }
	//bool operator != (const safe_enum & s) const { return this->val != s.val; }
	bool operator <  (const safe_enum & s) const { return this->val <  s.val; }
	bool operator <= (const safe_enum & s) const { return this->val <= s.val; }
	bool operator >  (const safe_enum & s) const { return this->val >  s.val; }
	bool operator >= (const safe_enum & s) const { return this->val >= s.val; }
	operator int() const { return (int)this->val; }
};

#define NXNA_ENUM(e) struct e##_def { enum type {
#define NXNA_BITWISE_ENUM(e) struct e##_def { enum type {
#define END_NXNA_ENUM(e) }; }; typedef safe_enum<e##_def> e;
#endif

// create some macros to disable constant warnings about "override" keyword
#ifdef _MSC_VER
#define NXNA_DISABLE_OVERRIDE_WARNING __pragma(warning(push)) \
__pragma(warning(disable:4481))
#define NXNA_ENABLE_OVERRIDE_WARNING __pragma(warning(pop))
#else
#define NXNA_DISABLE_OVERRIDE_WARNING
#define NXNA_ENABLE_OVERRIDE_WARNING
#endif

// create some macros to disable constant warnings about "nameless struct/union"
#ifdef _MSC_VER
#define NXNA_DISABLE_NAMELESS_STRUCT_WARNING __pragma(warning(push)) \
__pragma(warning(disable:4201))
#define NXNA_ENABLE_NAMELESS_STRUCT_WARNING __pragma(warning(pop))
#else
#define NXNA_DISABLE_NAMELESS_STRUCT_WARNING
#define NXNA_ENABLE_NAMELESS_STRUCT_WARNING
#endif

// make sure the SDL libraries get included if needed
#if defined NXNA_PLATFORM_WIN32 && defined NXNA_PLATFORMENGINE_SDL
#pragma comment(lib, "SDL2")
#pragma comment(lib, "SDL2main")
#endif

// don't do excessive error OpenGL error checking in non-debug builds
// (this only has an effect when using an OpenGL renderer)
#ifdef NDEBUG
#define NXNA_DISABLE_OPENGL_ERRORS
#endif

#define NXNA_DISABLE_OPENGL_ERRORS

// if for some reason you don't want Ogg Vorbis (.ogg files) support you can uncomment the following line
//#define NXNA_DISABLE_OGG

// if you don't want Direct3D 11 support you can uncomment the following line
//#define NXNA_DISABLE_D3D11

// if you don't want any audio then uncomment the following lines
//#undef NXNA_AUDIOENGINE_OPENAL
//#undef NXNA_AUDIOENGINE_OPENSL
//#define NXNA_AUDIOENGINE_NONE

#if defined NXNA_PLATFORM_APPLE_IOS
#define NXNA_DISABLE_OGG
#endif

#if !defined NXNA_PLATFORM_WIN32
#define NXNA_DISABLE_D3D11
#endif

#endif // NXNACONFIG_H
