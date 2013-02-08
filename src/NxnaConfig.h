#ifndef NXNACONFIG_H
#define NXNACONFIG_H

#include "Platform/PlatformDefs.h"

typedef unsigned char byte;

// are we using OpenGL ES?
#if defined NXNA_PLATFORM_APPLE_IOS || defined NXNA_PLATFORM_NACL || defined NXNA_PLATFORM_ANDROID
#define USING_OPENGLES
#endif

// disable Visual C++ 2010 specific stuff if needed
#if _MSC_VER != 1600
#define nullptr 0
#define override
#endif

// allow typesafe enums
#if _MSC_VER == 1700
#define NXNA_ENUM(e) enum class e {
#define END_NXNA_ENUM(e) };
#else
template<typename def, typename inner = typename def::type>
class safe_enum : public def
{
	typedef typename def::type type;
	inner val;
public:
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

// make sure the SDL libraries get included if needed
#if defined NXNA_PLATFORM_WIN32_SDL
#pragma comment(lib, "SDL")
#pragma comment(lib, "SDLMain")
#endif

// if for some reason you don't want Ogg Vorbis (.ogg files) support you can uncomment the following line
//#define NXNA_DISABLE_OGG

// if you don't want Direct3D 11 support you can uncomment the following line
//#define NXNA_DISABLE_D3D11

#if defined NXNA_PLATFORM_APPLE_IOS
#define NXNA_DISABLE_OGG
#endif

#if !defined NXNA_PLATFORM_WIN32
#define NXNA_DISABLE_D3D11
#endif

#endif // NXNACONFIG_H
