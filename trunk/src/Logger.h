#ifndef NXNA_LOGGER_H
#define NXNA_LOGGER_H

#include "NxnaConfig.h"

#if defined NXNA_PLATFORM_ANDROID

#include <android/log.h>

#define NXNA_LOG_DEBUG(...) __android_log_print(ANDROID_LOG_DEBUG, "nxna", __VA_ARGS__)
#else

#include <cstdio>

#define NXNA_LOG_DEBUG(...) printf(__VA_ARGS__)
#endif

#endif // NXNA_LOGGER_H
