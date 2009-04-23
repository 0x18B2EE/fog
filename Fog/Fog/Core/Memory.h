// [Fog/Core Library - C++ API]
//
// [Licence] 
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_MEMORY_H
#define _FOG_CORE_MEMORY_H

// [Dependencies]
#include <Fog/Build/Build.h>

#if defined(FOG_CC_MSVC)
#pragma intrinsic (_byteswap_ushort, _byteswap_ulong, _byteswap_uint64)
#endif // FOG_CC_MSVC

//! @addtogroup Fog_Core_CAPI
//! @{

// ============================================================================
// [Fog::Memory]
// ============================================================================

FOG_CAPI_EXTERN void* Core_Memory_alloc(sysuint_t size);
FOG_CAPI_EXTERN void* Core_Memory_calloc(sysuint_t size);
FOG_CAPI_EXTERN void* Core_Memory_realloc(void* addr, sysuint_t size);
FOG_CAPI_EXTERN void* Core_Memory_reallocf(void* addr, sysuint_t size);

FOG_CAPI_EXTERN void* Core_Memory_xalloc(sysuint_t size);
FOG_CAPI_EXTERN void* Core_Memory_xcalloc(sysuint_t size);
FOG_CAPI_EXTERN void* Core_Memory_xrealloc(void* addr, sysuint_t size);

FOG_CAPI_EXTERN void Core_Memory_free(void *addr);

FOG_CAPI_EXTERN void* Core_Memory_dup(void *addr, sysuint_t size);
FOG_CAPI_EXTERN void* Core_Memory_xdup(void *addr, sysuint_t size);

FOG_CVAR_EXTERN void (*Core_Memory_copy)(void* dest, const void* src, sysuint_t size);
FOG_CVAR_EXTERN void (*Core_Memory_move)(void* dest, const void* src, sysuint_t size);
FOG_CVAR_EXTERN void (*Core_Memory_set)(void* dest, uint c, sysuint_t size);
FOG_CVAR_EXTERN void (*Core_Memory_zero)(void* dest, sysuint_t size);

FOG_CVAR_EXTERN void (*Core_Memory_copy_nt)(void* dest, const void* src, sysuint_t size);
FOG_CVAR_EXTERN void (*Core_Memory_move_nt)(void* dest, const void* src, sysuint_t size);
FOG_CVAR_EXTERN void (*Core_Memory_set_nt)(void* dest, uint c, sysuint_t size);
FOG_CVAR_EXTERN void (*Core_Memory_zero_nt)(void* dest, sysuint_t size);

FOG_CAPI_EXTERN void Core_Memory_xchg(uint8_t* addr1, uint8_t* addr2, sysuint_t count);

// ============================================================================
// [Fog::MemoryDebugger]
// ============================================================================

FOG_CAPI_EXTERN void Core_MemoryDebugger_init(void);
FOG_CAPI_EXTERN void Core_MemoryDebugger_shutdown(void);
FOG_CAPI_EXTERN void Core_MemoryDebugger_add(void* addr, sysuint_t size);
FOG_CAPI_EXTERN void Core_MemoryDebugger_remove(void* addr);
FOG_CAPI_EXTERN void Core_MemoryDebugger_leaks(void);

//! @}

//! @addtogroup Fog_Core
//! @{

namespace Fog {

// [Fog::Uint64Union]

union UInt64Union
{
#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  struct
  {
    uint32_t u32low;
    uint32_t u32high;
  };
  struct 
  {
    uint32_t i32low;
    uint32_t i32high;
  };
#else
  struct
  {
    uint32_t u32high;
    uint32_t u32low;
  };
  struct 
  {
    uint32_t i32high;
    uint32_t i32low;
  };
#endif

  uint64_t u64;
  int64_t i64;
};

// [Fog::Memory]

//! @brief Memory related functions.
//!
//! @c new() and @c delete() in @c Core namespace are modified to call
//! @c Memory::xmalloc() and @c Memory::free().
struct Memory
{
  // [Alloc / Free]

  static FOG_INLINE void* alloc(sysuint_t size)
  {
    return Core_Memory_alloc(size);
  }

  static FOG_INLINE void* calloc(sysuint_t size)
  {
    return Core_Memory_calloc(size);
  }

  static FOG_INLINE void* realloc(void* addr, sysuint_t size)
  {
    return Core_Memory_realloc(addr, size);
  }

  static FOG_INLINE void* reallocf(void* addr, sysuint_t size)
  {
    return Core_Memory_reallocf(addr, size);
  }

  static FOG_INLINE void* xalloc(sysuint_t size)
  {
    return Core_Memory_xalloc(size);
  }

  static FOG_INLINE void* xcalloc(sysuint_t size)
  {
    return Core_Memory_xcalloc(size);
  }

  static FOG_INLINE void* xrealloc(void* addr, sysuint_t size)
  {
    return Core_Memory_xrealloc(addr, size);
  }

  static FOG_INLINE void free(void* addr)
  {
    return Core_Memory_free(addr);
  }

  // [Memory operations]

  static FOG_INLINE void* dup(void* addr, sysuint_t size)
  {
    return Core_Memory_dup(addr, size);
  }

  static FOG_INLINE void* xdup(void* addr, sysuint_t size)
  {
    return Core_Memory_xdup(addr, size);
  }

  static FOG_INLINE void copy(void* dest, const void* src, sysuint_t size)
  {
    Core_Memory_copy(dest, src, size);
  }

  static FOG_INLINE void move(void* dest, const void* src, sysuint_t size)
  {
    Core_Memory_move(dest, src, size);
  }

  static FOG_INLINE void set(void* dest, int c, sysuint_t size)
  {
    Core_Memory_set(dest, c, size);
  }

  static FOG_INLINE void zero(void* dest, sysuint_t size)
  {
    Core_Memory_zero(dest, size);
  }

  // [Memory operations - nt]

  static FOG_INLINE void copy_nt(void* dest, const void* src, sysuint_t size)
  {
    Core_Memory_copy_nt(dest, src, size);
  }

  static FOG_INLINE void move_nt(void* dest, const void* src, sysuint_t size)
  {
    Core_Memory_move_nt(dest, src, size);
  }

  static FOG_INLINE void set_nt(void* dest, int c, sysuint_t size)
  {
    Core_Memory_set_nt(dest, c, size);
  }

  static FOG_INLINE void zero_nt(void* dest, int c, sysuint_t size)
  {
    Core_Memory_zero_nt(dest, size);
  }

  // [copy::]

  static FOG_INLINE void copy1B(void* dest, const void* src)
  {
    ((uint8_t *)dest)[0] = ((const uint8_t *)src)[0];
  }

  static FOG_INLINE void copy2B(void* dest, const void* src)
  {
    ((uint16_t *)dest)[0] = ((const uint16_t *)src)[0];
  }

  static FOG_INLINE void copy3B(void* dest, const void* src)
  {
    ((uint16_t *)dest)[0] = ((const uint16_t *)src)[0];
    ((uint8_t  *)dest)[2] = ((const uint8_t  *)src)[2];
  }

  static FOG_INLINE void copy4B(void* dest, const void* src)
  {
    ((uint32_t *)dest)[0] = ((const uint32_t *)src)[0];
  }

  static FOG_INLINE void copy8B(void* dest, const void* src)
  {
#if FOG_ARCH_BITS == 32
    ((uint32_t *)dest)[0] = ((const uint32_t *)src)[0];
    ((uint32_t *)dest)[1] = ((const uint32_t *)src)[1];
#else
    ((uint64_t *)dest)[0] = ((const uint64_t *)src)[0];
#endif
  }

  static FOG_INLINE void copy16B(void* dest, const void* src)
  {
#if defined(FOG_HARDCODE_SSE2)
    _mm_storeu_si128((__m128i *)(dest), _mm_loadu_si128((__m128i *)(src)));
#elif FOG_ARCH_BITS == 64
    ((uint32_t *)dest)[0] = ((const uint32_t *)src)[0];
    ((uint32_t *)dest)[1] = ((const uint32_t *)src)[1];
    ((uint32_t *)dest)[2] = ((const uint32_t *)src)[2];
    ((uint32_t *)dest)[3] = ((const uint32_t *)src)[3];
#else
    ((uint64_t *)dest)[0] = ((const uint64_t *)src)[0];
    ((uint64_t *)dest)[1] = ((const uint64_t *)src)[1];
#endif
  }

  static FOG_INLINE void copy32B(void* dest, const void* src)
  {
#if defined(FOG_HARDCODE_SSE2)
    _mm_storeu_si128((__m128i *)(dest     ), _mm_loadu_si128((__m128i *)(src     )));
    _mm_storeu_si128((__m128i *)((char*)dest + 16), _mm_loadu_si128((__m128i *)((const char*)src + 16)));
#elif FOG_ARCH_BITS == 64
    ((uint64_t *)dest)[0] = ((const uint64_t *)src)[0];
    ((uint64_t *)dest)[1] = ((const uint64_t *)src)[1];
    ((uint64_t *)dest)[2] = ((const uint64_t *)src)[2];
    ((uint64_t *)dest)[3] = ((const uint64_t *)src)[3];
#else
    ((uint32_t *)dest)[0] = ((const uint32_t *)src)[0];
    ((uint32_t *)dest)[1] = ((const uint32_t *)src)[1];
    ((uint32_t *)dest)[2] = ((const uint32_t *)src)[2];
    ((uint32_t *)dest)[3] = ((const uint32_t *)src)[3];
    ((uint32_t *)dest)[4] = ((const uint32_t *)src)[4];
    ((uint32_t *)dest)[5] = ((const uint32_t *)src)[5];
    ((uint32_t *)dest)[6] = ((const uint32_t *)src)[6];
    ((uint32_t *)dest)[7] = ((const uint32_t *)src)[7];
#endif
  }

  static FOG_INLINE void copy64B(void* dest, const void* src)
  {
#if defined(FOG_HARDCODE_SSE2)
    _mm_storeu_si128((__m128i *)(dest     ), _mm_loadu_si128((__m128i *)(src     )));
    _mm_storeu_si128((__m128i *)((char*)dest + 16), _mm_loadu_si128((__m128i *)((const char*)src + 16)));
    _mm_storeu_si128((__m128i *)((char*)dest + 32), _mm_loadu_si128((__m128i *)((const char*)src + 32)));
    _mm_storeu_si128((__m128i *)((char*)dest + 48), _mm_loadu_si128((__m128i *)((const char*)src + 48)));
#elif FOG_ARCH_BITS == 64
    ((uint64_t *)dest)[0] = ((const uint64_t *)src)[0];
    ((uint64_t *)dest)[1] = ((const uint64_t *)src)[1];
    ((uint64_t *)dest)[2] = ((const uint64_t *)src)[2];
    ((uint64_t *)dest)[3] = ((const uint64_t *)src)[3];
    ((uint64_t *)dest)[4] = ((const uint64_t *)src)[4];
    ((uint64_t *)dest)[5] = ((const uint64_t *)src)[5];
    ((uint64_t *)dest)[6] = ((const uint64_t *)src)[6];
    ((uint64_t *)dest)[7] = ((const uint64_t *)src)[7];
#else
    ((uint32_t *)dest)[0] = ((const uint32_t *)src)[0];
    ((uint32_t *)dest)[1] = ((const uint32_t *)src)[1];
    ((uint32_t *)dest)[2] = ((const uint32_t *)src)[2];
    ((uint32_t *)dest)[3] = ((const uint32_t *)src)[3];
    ((uint32_t *)dest)[4] = ((const uint32_t *)src)[4];
    ((uint32_t *)dest)[5] = ((const uint32_t *)src)[5];
    ((uint32_t *)dest)[6] = ((const uint32_t *)src)[6];
    ((uint32_t *)dest)[7] = ((const uint32_t *)src)[7];
    ((uint32_t *)dest)[8] = ((const uint32_t *)src)[8];
    ((uint32_t *)dest)[9] = ((const uint32_t *)src)[9];
    ((uint32_t *)dest)[10] = ((const uint32_t *)src)[10];
    ((uint32_t *)dest)[11] = ((const uint32_t *)src)[11];
    ((uint32_t *)dest)[12] = ((const uint32_t *)src)[12];
    ((uint32_t *)dest)[13] = ((const uint32_t *)src)[13];
    ((uint32_t *)dest)[14] = ((const uint32_t *)src)[14];
    ((uint32_t *)dest)[15] = ((const uint32_t *)src)[15];
#endif
  }

  // [eq::]

  static FOG_INLINE bool eq1B(const void* a, const void* b)
  {
    return ((uint8_t *)a)[0] == ((uint8_t *)b)[0];
  }

  static FOG_INLINE bool eq2B(const void* a, const void* b)
  {
    return ((uint16_t *)a)[0] == ((uint16_t *)b)[0];
  }

  static FOG_INLINE bool eq3B(const void* a, const void* b)
  {
    return ((uint16_t *)a)[0] == ((uint16_t *)b)[0] &&
           ((uint8_t  *)a)[2] == ((uint8_t  *)b)[2] ;
  }

  static FOG_INLINE bool eq4B(const void* a, const void* b)
  {
    return ((uint32_t *)a)[0] == ((uint32_t *)b)[0];
  }

  static FOG_INLINE bool eq8B(const void* a, const void* b)
  {
#if FOG_ARCH_BITS == 64
    return ((uint64_t *)a)[0] == ((uint64_t *)b)[0];
#else
    return ((uint32_t *)a)[0] == ((uint32_t *)b)[0] &&
           ((uint32_t *)a)[1] == ((uint32_t *)b)[1] ;
#endif 
  }

  static FOG_INLINE bool eq16B(const void* a, const void* b)
  {
#if FOG_ARCH_BITS == 64
    return ((uint64_t *)a)[0] == ((uint64_t *)b)[0] &&
           ((uint64_t *)a)[1] == ((uint64_t *)b)[1] ;
#else
    return ((uint32_t *)a)[0] == ((uint32_t *)b)[0] &&
           ((uint32_t *)a)[1] == ((uint32_t *)b)[1] &&
           ((uint32_t *)a)[2] == ((uint32_t *)b)[2] &&
           ((uint32_t *)a)[3] == ((uint32_t *)b)[3] ;
#endif 
  }

  static FOG_INLINE bool eq32B(const void* a, const void* b)
  {
#if FOG_ARCH_BITS == 64
    return ((uint64_t *)a)[0] == ((uint64_t *)b)[0] &&
           ((uint64_t *)a)[1] == ((uint64_t *)b)[1] &&
           ((uint64_t *)a)[2] == ((uint64_t *)b)[2] &&
           ((uint64_t *)a)[3] == ((uint64_t *)b)[3] ;
#else
    return ((uint32_t *)a)[0] == ((uint32_t *)b)[0] &&
           ((uint32_t *)a)[1] == ((uint32_t *)b)[1] &&
           ((uint32_t *)a)[2] == ((uint32_t *)b)[2] &&
           ((uint32_t *)a)[3] == ((uint32_t *)b)[3] &&
           ((uint32_t *)a)[4] == ((uint32_t *)b)[4] &&
           ((uint32_t *)a)[5] == ((uint32_t *)b)[5] &&
           ((uint32_t *)a)[6] == ((uint32_t *)b)[6] &&
           ((uint32_t *)a)[7] == ((uint32_t *)b)[7] ;
#endif 
  }

  // [zero::]

  static FOG_INLINE void zero1B(void* a)
  {
    ((uint8_t*)a)[0] = 0;
  }

  static FOG_INLINE void zero2B(void* a)
  {
    ((uint16_t*)a)[0] = 0;
  }

  static FOG_INLINE void zero3B(void* a)
  {
    ((uint16_t*)a)[0] = 0;
    ((uint8_t *)a)[2] = 0;
  }

  static FOG_INLINE void zero4B(void* a)
  {
    ((uint32_t*)a)[0] = 0;
  }

  static FOG_INLINE void zero8B(void* a)
  {
#if FOG_ARCH_BITS == 64
    ((uint64_t*)a)[0] = FOG_UINT64_C(0);
#else
    ((uint32_t*)a)[0] = 0;
    ((uint32_t*)a)[1] = 0;
#endif
  }

  static FOG_INLINE void zero16B(void* a)
  {
#if defined(FOG_HARDCODE_SSE2)
    _mm_storeu_si128(&((__m128i *)a)[0], _mm_setzero_si128());
#elif FOG_ARCH_BITS == 64
    ((uint64_t*)a)[0] = FOG_UINT64_C(0);
    ((uint64_t*)a)[1] = FOG_UINT64_C(0);
#else
    ((uint32_t*)a)[0] = 0;
    ((uint32_t*)a)[1] = 0;
    ((uint32_t*)a)[2] = 0;
    ((uint32_t*)a)[3] = 0;
#endif
  }

  static FOG_INLINE void zero32B(void* a)
  {
#if defined(FOG_HARDCODE_SSE2)
    __m128i zero = _mm_setzero_si128();
    _mm_storeu_si128(&((__m128i *)a)[0], zero);
    _mm_storeu_si128(&((__m128i *)a)[1], zero);
#elif FOG_ARCH_BITS == 64
    ((uint64_t*)a)[0] = FOG_UINT64_C(0);
    ((uint64_t*)a)[1] = FOG_UINT64_C(0);
    ((uint64_t*)a)[2] = FOG_UINT64_C(0);
    ((uint64_t*)a)[3] = FOG_UINT64_C(0);
#else
    ((uint32_t*)a)[0] = 0;
    ((uint32_t*)a)[1] = 0;
    ((uint32_t*)a)[2] = 0;
    ((uint32_t*)a)[3] = 0;
    ((uint32_t*)a)[4] = 0;
    ((uint32_t*)a)[5] = 0;
    ((uint32_t*)a)[6] = 0;
    ((uint32_t*)a)[7] = 0;
#endif
  }

  // [xchg::]

  static FOG_INLINE void xchg1B(void* a, void* b)
  {
    register uint8_t t0 = ((uint8_t*)a)[0];

    ((uint8_t*)a)[0] = ((uint8_t*)b)[0];

    ((uint8_t*)b)[0] = t0;
  }

  static FOG_INLINE void xchg2B(void* a, void* b)
  {
    register uint16_t t0 = ((uint16_t*)a)[0];

    ((uint16_t*)a)[0] = ((uint16_t*)b)[0];

    ((uint16_t*)b)[0] = t0;
  }

  static FOG_INLINE void xchg3B(void* a, void* b)
  {
    register uint16_t t0 = ((uint16_t*)a)[0];
    register uint8_t  t1 = ((uint8_t *)a)[2];

    ((uint16_t*)a)[0] = ((uint16_t*)b)[0];
    ((uint8_t *)a)[2] = ((uint8_t *)b)[2];

    ((uint16_t*)b)[0] = t0;
    ((uint8_t *)b)[2] = t1;
  }

  static FOG_INLINE void xchg4B(void* a, void* b)
  {
    register uint32_t t0 = ((uint32_t*)a)[0];

    ((uint32_t*)a)[0] = ((uint32_t*)b)[0];

    ((uint32_t*)b)[0] = t0;
  }

  static FOG_INLINE void xchg8B(void* a, void* b)
  {
#if FOG_ARCH_BITS == 64
    register uint64_t t0 = ((uint64_t*)a)[0];

    ((uint64_t*)a)[0] = ((uint64_t*)b)[0];

    ((uint64_t*)b)[0] = t0;
#else
    register uint32_t t0 = ((uint32_t*)a)[0];
    register uint32_t t1 = ((uint32_t*)a)[1];

    ((uint32_t*)a)[0] = ((uint32_t*)b)[0];
    ((uint32_t*)a)[1] = ((uint32_t*)b)[1];

    ((uint32_t*)b)[0] = t0;
    ((uint32_t*)b)[1] = t1;
#endif
  }

  static FOG_INLINE void xchg16B(void* a, void* b)
  {
#if defined(FOG_HARDCODE_SSE2)
    __m128i t0a = _mm_loadu_si128((__m128i *)(a));
    __m128i t0b = _mm_loadu_si128((__m128i *)(b));

    _mm_storeu_si128((__m128i *)(b), t0a);
    _mm_storeu_si128((__m128i *)(a), t0b);
#elif FOG_ARCH_BITS == 64
    uint64_t t0 = ((uint64_t*)a)[0];
    uint64_t t1 = ((uint64_t*)a)[1];

    ((uint64_t*)a)[0] = ((uint64_t*)b)[0];
    ((uint64_t*)a)[1] = ((uint64_t*)b)[1];

    ((uint64_t*)b)[0] = t0;
    ((uint64_t*)b)[1] = t1;
#else
    uint32_t t0 = ((uint32_t*)a)[0];
    uint32_t t1 = ((uint32_t*)a)[1];
    uint32_t t2 = ((uint32_t*)a)[2];
    uint32_t t3 = ((uint32_t*)a)[3];

    ((uint32_t*)a)[0] = ((uint32_t*)b)[0];
    ((uint32_t*)a)[1] = ((uint32_t*)b)[1];
    ((uint32_t*)a)[2] = ((uint32_t*)b)[2];
    ((uint32_t*)a)[3] = ((uint32_t*)b)[3];

    ((uint32_t*)b)[0] = t0;
    ((uint32_t*)b)[1] = t1;
    ((uint32_t*)b)[2] = t2;
    ((uint32_t*)b)[3] = t3;
#endif
  }

  static FOG_INLINE void xchg32B(void* a, void* b)
  {
#if defined(FOG_HARDCODE_SSE2)
    __m128i t0a = _mm_loadu_si128( &(((__m128i *)a)[0]) );
    __m128i t1a = _mm_loadu_si128( &(((__m128i *)a)[1]) );
    __m128i t0b = _mm_loadu_si128( &(((__m128i *)b)[0]) );
    __m128i t1b = _mm_loadu_si128( &(((__m128i *)b)[1]) );

    _mm_storeu_si128(&(((__m128i *)b)[0]), t0a);
    _mm_storeu_si128(&(((__m128i *)b)[1]), t1a);
    _mm_storeu_si128(&(((__m128i *)a)[0]), t0b);
    _mm_storeu_si128(&(((__m128i *)a)[1]), t1b);
#elif FOG_ARCH_BITS == 64
    uint64_t t0 = ((uint64_t*)a)[0];
    uint64_t t1 = ((uint64_t*)a)[1];

    ((uint64_t*)a)[0] = ((uint64_t*)b)[0];
    ((uint64_t*)a)[1] = ((uint64_t*)b)[1];

    ((uint64_t*)b)[0] = t0;
    ((uint64_t*)b)[1] = t1;
#else
    uint32_t t0 = ((uint32_t*)a)[0];
    uint32_t t1 = ((uint32_t*)a)[1];
    uint32_t t2 = ((uint32_t*)a)[2];
    uint32_t t3 = ((uint32_t*)a)[3];

    ((uint32_t*)a)[0] = ((uint32_t*)b)[0];
    ((uint32_t*)a)[1] = ((uint32_t*)b)[1];
    ((uint32_t*)a)[2] = ((uint32_t*)b)[2];
    ((uint32_t*)a)[3] = ((uint32_t*)b)[3];

    ((uint32_t*)b)[0] = t0;
    ((uint32_t*)b)[1] = t1;
    ((uint32_t*)b)[2] = t2;
    ((uint32_t*)b)[3] = t3;
#endif
  }

  static FOG_INLINE void xchg(void* a, void* b, sysuint_t count)
  {
    Core_Memory_xchg((uint8_t*)a, (uint8_t*)b, count);
  }

  // [bswap]

  static FOG_INLINE uint16_t bswap16(uint16_t x)
  {
#if defined(FOG_CC_GNU) && defined(FOG_ARCH_X86)
    __asm__("xchgb %b0,%h0" : "=q" (x) :  "0" (x));
    return x;
#elif defined(FOG_CC_GNU) && defined(FOG_ARCH_X86_64)
    __asm__("xchgb %b0,%h0" : "=Q" (x) :  "0" (x));
    return x;
#elif defined(FOG_CC_GNU) && defined(FOG_ARCH_PPC)
    uint16_t result;
    __asm__("rlwimi %0,%2,8,16,23" : "=&r" (result) : "0" (x >> 8), "r" (x));
    return result;
#elif defined(FOG_CC_GNU) && defined(__M68000__)
    __asm__("rorw #8,%0" : "=d" (x) :  "0" (x) : "cc");
    return x;
#elif defined(FOG_CC_MSVC)
    return _byteswap_ushort(x);
#else
    return (x << 8) | (x >> 8) ;
#endif
  }

  static FOG_INLINE uint32_t bswap32(uint32_t x)
  {
#if defined(FOG_CC_GNU) && defined(FOG_ARCH_X86)
    __asm__("bswap %0" : "=r" (x) : "0" (x));
    return x;
#elif defined(FOG_CC_GNU) && defined(FOG_ARCH_X86_64)
    __asm__("bswapl %0" : "=r" (x) : "0" (x));
    return x;
#elif defined(FOG_CC_GNU) && defined(FOG_ARCH_PPC)
    uint32_t result;

    __asm__("rlwimi %0,%2,24,16,23" : "=&r" (result) : "0" (x>>24), "r" (x));
    __asm__("rlwimi %0,%2,8,8,15"   : "=&r" (result) : "0" (result), "r" (x));
    __asm__("rlwimi %0,%2,24,0,7"   : "=&r" (result) : "0" (result), "r" (x));
    return result;
#elif defined(FOG_CC_GNU) && defined(__M68000__)
    __asm__("rorw #8,%0\n\t"
        "swap %0\n\t"
        "ror #8,%0" : "=d" (x) :  "0" (x) : "cc");
    return x;
#elif defined(FOG_CC_MSVC) && (FOG_ARCH_BITS == 32)
    // I'm not sure about this intrinsics for the 64 bit platform,
    // Microsoft documentation says that it's long, but long should
    // be 64 bit on 64 bit arch...
    return _byteswap_ulong(x);
#else
    return (
      (x << 24) |
      (x >> 24) |
      ((x << 8) & 0x00FF0000) |
      ((x >> 8) & 0x0000FF00) );
#endif
  }

  static FOG_INLINE uint64_t bswap64(uint64_t x)
  {
#if defined(FOG_CC_GNU) && defined(FOG_ARCH_X86)
    UInt64Union v;
    v.u64 = x;
    __asm__("bswapl %0 ; bswapl %1 ; xchgl %0,%1"
        : "=r" (v.u32low), "=r" (v.u32high)
        : "0" (v.u32low), "1" (v.u32high));
    return v.u64;
#elif defined(FOG_CC_GNU) && defined(FOG_ARCH_X86_64)
    __asm__("bswapq %0" : "=r" (x) : "0" (x));
    return x;
#elif defined(FOG_CC_MSVC)
    return _byteswap_uint64(x);
#else
    uint32_t hi, lo;

    // Separate into high and low 32-bit values and swap them
    lo = (uint32_t)(x & 0xFFFFFFFF);
    x >>= 32;
    hi = (uint32_t)(x & 0xFFFFFFFF);
    x = bswap32(lo);
    x <<= 32;
    x |= bswap32(hi);
    return(x);
#endif
  }

  static FOG_INLINE int16_t bswap16(int16_t x)
  { return (int16_t)bswap16( (uint16_t)x ); }

  static FOG_INLINE int32_t bswap32(int32_t x)
  { return (int32_t)bswap32( (uint32_t)x ); }

  static FOG_INLINE int64_t bswap64(int64_t x)
  { return (uint64_t)bswap64( (uint64_t)x ); }

#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
#define __FOG_SWAP(__le__, __be__) __le__
#else
#define __FOG_SWAP(__le__, __be__) __be__
#endif

  static FOG_INLINE uint16_t bswap16le(uint16_t x) { return __FOG_SWAP(x, bswap16(x)); }
  static FOG_INLINE uint16_t bswap16le(int16_t x)  { return __FOG_SWAP(x, bswap16(x)); }
  static FOG_INLINE uint32_t bswap32le(uint32_t x) { return __FOG_SWAP(x, bswap32(x)); }
  static FOG_INLINE uint32_t bswap32le(int32_t x)  { return __FOG_SWAP(x, bswap32(x)); }
  static FOG_INLINE uint64_t bswap64le(uint64_t x) { return __FOG_SWAP(x, bswap64(x)); }
  static FOG_INLINE uint64_t bswap64le(int64_t x)  { return __FOG_SWAP(x, bswap64(x)); }

  static FOG_INLINE uint16_t bswap16be(uint16_t x) { return __FOG_SWAP(bswap16(x), x); }
  static FOG_INLINE uint16_t bswap16be(int16_t x)  { return __FOG_SWAP(bswap16(x), x); }
  static FOG_INLINE uint32_t bswap32be(uint32_t x) { return __FOG_SWAP(bswap32(x), x); }
  static FOG_INLINE uint32_t bswap32be(int32_t x)  { return __FOG_SWAP(bswap32(x), x); }
  static FOG_INLINE uint64_t bswap64be(uint64_t x) { return __FOG_SWAP(bswap64(x), x); }
  static FOG_INLINE uint64_t bswap64be(int64_t x)  { return __FOG_SWAP(bswap64(x), x); }

#undef __FOG_SWAP

};

} // Fog namespace

//! @}

// [Guard]
#endif // _FOG_CORE_MEMORY_H