#ifndef EVOAI_CONFIG_HPP
#define EVOAI_CONFIG_HPP

#define EVOAI_VERSION_MAJOR 1
#define EVOAI_VERSION_MINOR 0
#define EVOAI_VERSION_PATCH 0

#if !defined(EvoAI_BUILD_STATIC)
    #if defined(_WIN32)
        // Windows compilers need specific (and different) keywords for export and import
        #define EvoAI_API_EXPORT __declspec(dllexport)
        #define EvoAI_API_IMPORT __declspec(dllimport)
    #else // Linux, FreeBSD, Mac OS X
        #if __GNUC__ >= 4
            // GCC 4 has special keywords for showing/hidding symbols,
            // the same keyword is used for both importing and exporting
            #define EvoAI_API_EXPORT __attribute__ ((__visibility__ ("default")))
            #define EvoAI_API_IMPORT __attribute__ ((__visibility__ ("default")))
        #else
            // GCC < 4 has no mechanism to explicitely hide symbols, everything's exported
            #define EvoAI_API_EXPORT
            #define EvoAI_API_IMPORT
        #endif
    #endif
#else
    // Static build doesn't need import/export macros
    #define EvoAI_API_EXPORT
    #define EvoAI_API_IMPORT
#endif
#endif // EVOAI_CONFIG_HPP
