#ifndef EVOAI_EXPORT_HPP
#define EVOAI_EXPORT_HPP

#include <EvoAI/Config.hpp>
    // Define portable import / export macros
    #if defined(EvoAI_EXPORTS)
        #define EvoAI_API EvoAI_API_EXPORT
    #else
        #define EvoAI_API EvoAI_API_IMPORT
    #endif
#endif // EVOAI_EXPORT_HPP