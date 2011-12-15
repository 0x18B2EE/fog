// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

#ifndef _FOG_CORE_CPP_STDARCH_H
#define _FOG_CORE_CPP_STDARCH_H

#if !defined(FOG_IDE) && !defined(_FOG_CORE_CPP_BASE_H)
#error "Fog/Core/C++/StdArch.h can be only included by Fog/Core/C++/Base.h"
#endif // _FOG_CORE_CPP_BASE_H

// ============================================================================
// [Fog::Core::C++ - Architecture]
// ============================================================================

//! @addtogroup Fog_Core_Cpp
//! @{

//! @def FOG_ARCH_X86
//! @brief x86 compatible, can be i386 or higher.

//! @def FOG_ARCH_X86_64
//! @brief based on 64 bit intel or 64 bit AMD processor.

//! @def FOG_ARCH_PPC
//! @brief 32 bit powerPC.

//! @def FOG_ARCH_BITS
//! @brief How bits has target CPU (32 or 64).
//!
//! Very nice method to check whether it's faster to use 64-bit arithmetics.

//! @def FOG_HARDCODE_MMX
//! @brief If defined, MMX assembly will be hardcoded into binaries and no lower optimizations are allowed.

//! @def FOG_HARDCODE_MMX2
//! @brief If defined, MMX2 assembly will be hardcoded into binaries and no lower optimizations are allowed.

//! @def FOG_HARDCODE_SSE
//! @brief If defined, SSE assembly will be hardcoded into binaries and no lower optimizations are allowed.

//! @def FOG_HARDCODE_SSE2
//! @brief If defined, SSE2 assembly will be hardcoded into binaries and no lower optimizations are allowed.

//! @def FOG_HARDCODE_SSE3
//! @brief If defined, SSE3 assembly will be hardcoded into binaries and no lower optimizations are allowed.

// ============================================================================
// [Fog::Core::C++ - Architecture - X86_64]
// ============================================================================

#if defined(_M_X64        ) || \
    defined(_M_AMD64      ) || \
    defined(__x86_64__    ) || \
    defined(__x86_64      ) || \
    defined(__amd64__     ) || \
    defined(__amd64       )

# define FOG_ARCH_X86_64
# define FOG_ARCH_BITS 64

// x86_64 allows unaligned access.
# define FOG_ARCH_UNALIGNED_ACCESS_32
# define FOG_ARCH_UNALIGNED_ACCESS_64

// x86_64 uses always SSE/SSE2
# define FOG_HARDCODE_SSE
# define FOG_HARDCODE_SSE2

// ============================================================================
// [Fog::Core::C++ - Architecture - X86_32]
// ============================================================================

#elif defined(_M_IX86     ) || \
      defined(__i386__    ) || \
      defined(__i386      ) || \
      defined(__i486__    ) || \
      defined(__i586__    ) || \
      defined(__i686__    )

# define FOG_ARCH_X86
# define FOG_ARCH_BITS 32

// x86 allows unaligned access.
# define FOG_ARCH_UNALIGNED_ACCESS_32
# define FOG_ARCH_UNALIGNED_ACCESS_64

// GCC specific.
# if defined(__MMX__)
#  define FOG_HARDCODE_MMX
# endif // __i686__

// MSVC defines this if compiler optimizes code for MMX or SSE.
# if defined(_M_IX86_FP)
#  if _M_IX86_FP == 1
#   define FOG_HARDCODE_MMX
#   define FOG_HARDCODE_MMX2
#  elif _M_IX86_FP == 2
#   define FOG_HARDCODE_SSE
#   define FOG_HARDCODE_SSE2
#  endif
# endif // _M_IX86_FP

// ============================================================================
// [Fog::Core::C++ - Architecture - Itanium]
// ============================================================================

#elif defined(_M_IA64)

# define FOG_ARCH_ITANIUM
# define FOG_ARCH_BITS 64

// Unaligned access on itanium is only possible for 32-bit integer, not for
// pointer type, which is 64-bit wide.
# define FOG_ARCH_UNALIGNED_ACCESS_32

// ============================================================================
// [Fog::Core::C++ - Architecture - ARM]
// ============================================================================

#elif defined(__M_ARM     ) || \
      defined(__arm       ) || \
      defined(__arm__     ) || \
      defined(__ARM_NEON__)

# define FOG_ARCH_ARM
# define FOG_ARCH_BITS 32

# if defined(__ARM_NEON__)
#  define FOG_ARCH_ARM_NEON
# endif // __ARM_NEON__

// ARM doesn't allow unaligned access or it's very slow. We disable it and make
// it prohibited.

// ============================================================================
// [Fog::Core::C++ - Architecture - PowerPC]
// ============================================================================

#elif defined(_M_PPC      ) || \
      defined(_M_MPPC     ) || \
      defined(__ppc__     ) || \
      defined(__powerpc__ )

# define FOG_ARCH_PPC
# define FOG_ARCH_BITS 32

// PowerPC allows unaligned access of 32-bit data, but prohibits unaligned 
// access of 64-bit data.
# define FOG_ARCH_UNALIGNED_ACCESS_32

// ============================================================================
// [Fog::Core::C++ - Hack for IDE]
// ============================================================================

#elif defined(FOG_IDE)

# define FOG_ARCH_X86
# define FOG_ARCH_BITS 32

#else
// If you know your architecture and you want to try compile
// Fog library. Uncomment error and declare following macro:
// #define FOG_ARCH_BITS 32 or 64
# error "Fog::Core::C++ - Unsupported CPU, please fill a bug report."
#endif

#define FOG_BIG_ENDIAN 0
#define FOG_LITTLE_ENDIAN 1

#if FOG_ARCH_BITS == 32
# define FOG_ARCH_32_64(code32, code64) code32
#else
# define FOG_ARCH_32_64(code32, code64) code64
#endif // FOG_ARCH_BITS

//! @}

#endif // _FOG_CORE_CPP_STDARCH_H