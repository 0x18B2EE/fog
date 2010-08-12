// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// For some IDEs to enable code-assist.
#include <Fog/Core/Build.h>

#if defined(FOG_IDE)
#include <Fog/Graphics/RasterEngine/Defs_C_p.h>
#endif // FOG_IDE

namespace Fog {
namespace RasterEngine {

// ============================================================================
// [Fog::RasterEngine::C - Composite - SrcAtop]
// ============================================================================

//! @internal
struct FOG_HIDDEN CompositeSrcAtopC : public CompositeBaseFuncsC32<CompositeSrcAtopC>
{
  enum { CHARACTERISTICS = OPERATOR_CHAR_SRC_ATOP };

  static FOG_INLINE void prgb32_op_prgb32_32b(
    Face::b32_1x2& dst0, Face::b32_1x2 a0, Face::b32_1x2 b0,
    Face::b32_1x2& dst1, Face::b32_1x2 a1, Face::b32_1x2 b1)
  {
    uint32_t aa = Face::b32_1x2GetB1(a1);
    uint32_t bainv = Face::u32Negate255(Face::b32_1x2GetB1(b1));

    Face::b32_2x2MulDiv255U_2x_Add(
      dst0, a0, bainv, b0, aa,
      dst1, a1, bainv, b1, aa);
  }

  static FOG_INLINE void prgb32_op_xrgb32_32b(
    Face::b32_1x2& dst0, Face::b32_1x2 a0, Face::b32_1x2 b0,
    Face::b32_1x2& dst1, Face::b32_1x2 a1, Face::b32_1x2 b1)
  {
    // NOT USED!
    FOG_ASSERT_NOT_REACHED();
  }

  static FOG_INLINE void xrgb32_op_prgb32_32b(
    Face::b32_1x2& dst0, Face::b32_1x2 a0, Face::b32_1x2 b0,
    Face::b32_1x2& dst1, Face::b32_1x2 a1, Face::b32_1x2 b1)
  {
    // NOT USED!
    FOG_ASSERT_NOT_REACHED();
  }

  static FOG_INLINE void xrgb32_op_xrgb32_32b(
    Face::b32_1x2& dst0, Face::b32_1x2 a0, Face::b32_1x2 b0,
    Face::b32_1x2& dst1, Face::b32_1x2 a1, Face::b32_1x2 b1)
  {
    // NOT USED!
    FOG_ASSERT_NOT_REACHED();
  }
};

} // RasterEngine namespace
} // Fog namespace