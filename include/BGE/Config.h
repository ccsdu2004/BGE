#ifndef BGE_CONFIG_H
#define BGE_CONFIG_H
#include <stdint.h>

#define BGE_VERSION_MAJOR 0
#define BGE_VERSION_MINOR 6
#define BGE_VERSION_PATCH 0

#if defined(WIN32) || defined(WIN64)
    // Windows
    #define BGE_SYSTEM_WINDOWS
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
#elif defined(linux) || defined(__linux)
    // Linux
    #define BGE_SYSTEM_LINUX
#elif defined(__APPLE__) || defined(MACOSX) || defined(macintosh) || defined(Macintosh)
    // MacOS
    #define BGE_SYSTEM_MACOS
#elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
    // FreeBSD
    #define BGE_SYSTEM_FREEBSD
#else
    // Unsupported system
    #error this operating system is not supported by BGE library
#endif

#if !defined(NDEBUG)
    #define BGE_DEBUG
#endif

#if !defined(BGE_STATIC)
    #if defined(BGE_SYSTEM_WINDOWS)
        // Windows compilers need specific (and different) keywords for export and import
        #define BGE_API_EXPORT __declspec(dllexport)
        #define BGE_API_IMPORT __declspec(dllimport)
        // For Visual C++ compilers, we also need to turn off this annoying C4251 warning
        #ifdef _MSC_VER
            #pragma warning(disable : 4251)
        #endif
    #else // Linux, FreeBSD, Mac OS X
        #if __GNUC__ >= 4
            // GCC 4 has special keywords for showing/hidding symbols,
            // the same keyword is used for both importing and exporting
            #define BGE_API_EXPORT __attribute__ ((__visibility__ ("default")))
            #define BGE_API_IMPORT __attribute__ ((__visibility__ ("default")))
        #else
            // GCC < 4 has no mechanism to explicitely hide symbols, everything's exported
            #define BGE_API_EXPORT
            #define BGE_API_IMPORT
        #endif
    #endif
#else
    // Static build doesn't need import/export macros
    #define BGE_API_EXPORT
    #define BGE_API_IMPORT
#endif

#if defined(BGE_EXPORTS)
    #define BGE_EXPORT_API BGE_API_EXPORT
#else
    #define BGE_EXPORT_API BGE_API_IMPORT
#endif

#endif
