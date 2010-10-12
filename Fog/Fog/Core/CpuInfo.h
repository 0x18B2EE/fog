// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// Copyright (c) 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// [Guard]
#ifndef _FOG_CORE_CPUINFO_H
#define _FOG_CORE_CPUINFO_H

// [Dependencies]
#include <Fog/Core/Build.h>
#include <Fog/Core/Constants.h>

namespace Fog {

//! @addtogroup Fog_Core_Environment
//! @{

// ============================================================================
// [Fog::CpuId]
// ============================================================================

union FOG_HIDDEN CpuId
{
  uint32_t i[4];
  struct
  {
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
  };
};

// ============================================================================
// [Fog::CpuInfo]
// ============================================================================

//! @brief Information about CPU features.
//!
//! Easy way to read CPU features. CPU is detected in startup time, so this class
//! has method only to read the detected state. Use @c Cpu::features to 
//! retrieve information about your CPU and @c use Cpu::Features enumeration 
//! to get interesting features for you.
//!
//! Small example how to check for SSE2 support.
//!
//! @verbatim
//! if (Fog::CpuInfo::get()->hasFeature(Fog::CpuInfo::FEATURE_SSE2))
//! {
//!   // cpu has SSE2 support
//! }
//! else
//! {
//!   // cpu hasn't SSE2 support
//! }
//! @endverbatim
struct FOG_HIDDEN CpuInfo
{
  //! @brief Cpu short vendor string.
  char vendor[16];
  //! @brief Cpu long vendor string (brand).
  char brand[64];
  //! @brief Cpu family ID.
  uint32_t family;
  //! @brief Cpu model ID.
  uint32_t model;
  //! @brief Cpu stepping.
  uint32_t stepping;
  //! @brief Number of processors or cores.
  uint32_t numberOfProcessors;
  //! @brief Cpu features bitfield, see @c AsmJit::CpuInfo::FEATURE enum).
  uint32_t features;
  //! @brief Cpu bugs bitfield, see @c AsmJit::CpuInfo::BUG enum).
  uint32_t bugs;

  FOG_INLINE bool hasFeature(uint32_t feature) const
  { return (this->features & feature) != 0; }

  FOG_INLINE bool hasFeatures(uint32_t features) const
  { return (this->features & features) == features; }

  FOG_INLINE bool hasBug(uint32_t bug) const
  { return (this->bugs & bug) != 0; }

  static FOG_INLINE const CpuInfo* get();
};

FOG_CVAR_EXTERN CpuInfo fog_cpuinfo;

FOG_INLINE const CpuInfo* CpuInfo::get() { return &fog_cpuinfo; }

#if defined(FOG_ARCH_X86) || defined(FOG_ARCH_X86_64)
//! @brief Retrieve CPUID values.
FOG_API void cpuid(uint32_t in, CpuId* out);
#endif // FOG_ARCH_X86) || FOG_ARCH_X86_64

//! @brief Detect CPU info to @a i.
FOG_API void detectCpuInfo(CpuInfo* i);

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_CPUINFO_H
