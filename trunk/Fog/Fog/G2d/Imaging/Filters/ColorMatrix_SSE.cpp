// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Face/FaceSSE.h>
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Tools/Cpu.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Imaging/Filters/ColorMatrix.h>
#include <Fog/G2d/Source/Color.h>

namespace Fog {

// ============================================================================
// [Fog::ColorMatrix - Add]
// ============================================================================

static err_t FOG_CDECL ColorMatrix_addMatrix_SSE(ColorMatrix* dst, const ColorMatrix* a, const ColorMatrix* b)
{
  float *dm = dst->m;
  const float *am = a->m;
  const float *bm = b->m;

  Face::m128f xmm0a, xmm1a, xmm2a, xmm3a;
  Face::m128f xmm0b, xmm1b, xmm2b, xmm3b;

  Face::m128fLoad16uLoHi(xmm0a, am +  0);
  Face::m128fLoad16uLoHi(xmm1a, am +  4);
  Face::m128fLoad16uLoHi(xmm2a, am +  8);
  Face::m128fLoad16uLoHi(xmm3a, am + 12);

  Face::m128fLoad16uLoHi(xmm0b, bm +  0);
  Face::m128fLoad16uLoHi(xmm1b, bm +  4);
  Face::m128fLoad16uLoHi(xmm2b, bm +  8);
  Face::m128fLoad16uLoHi(xmm3b, bm + 12);

  Face::m128fAddPS(xmm0a, xmm0a, xmm0b);
  Face::m128fAddPS(xmm1a, xmm1a, xmm1b);

  Face::m128fStore16uLoHi(dm +  0, xmm0a);
  Face::m128fStore16uLoHi(dm +  4, xmm1a);

  Face::m128fLoad16uLoHi(xmm0a, am + 16);
  Face::m128fLoad16uLoHi(xmm1a, am + 20);

  Face::m128fAddPS(xmm2a, xmm2a, xmm2b);
  Face::m128fAddPS(xmm3a, xmm3a, xmm3b);

  Face::m128fLoad16uLoHi(xmm0b, bm + 16);
  Face::m128fLoad16uLoHi(xmm1b, bm + 20);

  Face::m128fStore16uLoHi(dm +  8, xmm2a);
  Face::m128fStore16uLoHi(dm + 12, xmm3a);

  Face::m128fLoad4(xmm2a, am + 24);
  Face::m128fLoad4(xmm2b, bm + 24);

  Face::m128fAddPS(xmm0a, xmm0a, xmm0b);
  Face::m128fAddPS(xmm1a, xmm1a, xmm1b);

  Face::m128fStore16uLoHi(dm + 16, xmm0a);

  Face::m128fAddSS(xmm2a, xmm2a, xmm2b);
  Face::m128fStore16uLoHi(dm + 20, xmm1a);
  Face::m128fStore4(dm + 24, xmm2a);

  return ERR_OK;
}

// ============================================================================
// [Fog::ColorMatrix - Subtract]
// ============================================================================

static err_t FOG_CDECL ColorMatrix_subtractMatrix_SSE(ColorMatrix* dst, const ColorMatrix* a, const ColorMatrix* b)
{
  float *dm = dst->m;
  const float *am = a->m;
  const float *bm = b->m;

  Face::m128f xmm0a, xmm1a, xmm2a, xmm3a;
  Face::m128f xmm0b, xmm1b, xmm2b, xmm3b;

  Face::m128fLoad16uLoHi(xmm0a, am +  0);
  Face::m128fLoad16uLoHi(xmm1a, am +  4);
  Face::m128fLoad16uLoHi(xmm2a, am +  8);
  Face::m128fLoad16uLoHi(xmm3a, am + 12);

  Face::m128fLoad16uLoHi(xmm0b, bm +  0);
  Face::m128fLoad16uLoHi(xmm1b, bm +  4);
  Face::m128fLoad16uLoHi(xmm2b, bm +  8);
  Face::m128fLoad16uLoHi(xmm3b, bm + 12);

  Face::m128fSubPS(xmm0a, xmm0a, xmm0b);
  Face::m128fSubPS(xmm1a, xmm1a, xmm1b);

  Face::m128fStore16uLoHi(dm +  0, xmm0a);
  Face::m128fStore16uLoHi(dm +  4, xmm1a);

  Face::m128fLoad16uLoHi(xmm0a, am + 16);
  Face::m128fLoad16uLoHi(xmm1a, am + 20);

  Face::m128fSubPS(xmm2a, xmm2a, xmm2b);
  Face::m128fSubPS(xmm3a, xmm3a, xmm3b);

  Face::m128fLoad16uLoHi(xmm0b, bm + 16);
  Face::m128fLoad16uLoHi(xmm1b, bm + 20);

  Face::m128fStore16uLoHi(dm +  8, xmm2a);
  Face::m128fStore16uLoHi(dm + 12, xmm3a);

  Face::m128fLoad4(xmm2a, am + 24);
  Face::m128fLoad4(xmm2b, bm + 24);

  Face::m128fSubPS(xmm0a, xmm0a, xmm0b);
  Face::m128fSubPS(xmm1a, xmm1a, xmm1b);

  Face::m128fStore16uLoHi(dm + 16, xmm0a);

  Face::m128fSubSS(xmm2a, xmm2a, xmm2b);
  Face::m128fStore16uLoHi(dm + 20, xmm1a);
  Face::m128fStore4(dm + 24, xmm2a);

  return ERR_OK;
}

// ============================================================================
// [Fog::ColorMatrix - Map]
// ============================================================================

static void FOG_CDECL ColorMatrix_mapArgbF_SSE(const ColorMatrix* self, ArgbF* dst, const ArgbF* src)
{
  const float* m = self->m;

  Face::m128f xmmR, xmmG, xmmB, xmmA;
  Face::m128f xmm0, xmm1, xmm2, xmm3;

  Face::m128fLoad16uLoHi(xmm0, m +  0);
  Face::m128fLoad16uLoHi(xmm1, m +  5);
  Face::m128fLoad16uLoHi(xmm2, m + 10);
  Face::m128fLoad16uLoHi(xmm3, m + 15);

  Face::m128fLoad4(xmmA, &src->a);
  Face::m128fLoad4(xmmR, &src->r);
  Face::m128fLoad4(xmmG, &src->g);
  Face::m128fLoad4(xmmB, &src->b);

  Face::m128fShuffle<0, 0, 0, 0>(xmmR, xmmR, xmmR);
  Face::m128fShuffle<0, 0, 0, 0>(xmmG, xmmG, xmmG);

  Face::m128fMulPS(xmm0, xmm0, xmmR);
  Face::m128fMulPS(xmm1, xmm1, xmmG);

  Face::m128fShuffle<0, 0, 0, 0>(xmmB, xmmB, xmmB);
  Face::m128fShuffle<0, 0, 0, 0>(xmmA, xmmA, xmmA);

  Face::m128fMulPS(xmm2, xmm2, xmmB);
  Face::m128fMulPS(xmm3, xmm3, xmmA);

  Face::m128fAddPS(xmm0, xmm0, xmm1);
  Face::m128fLoad16uLoHi(xmm1, m + 20);

  Face::m128fAddPS(xmm2, xmm2, xmm3);
  Face::m128fAddPS(xmm0, xmm0, xmm1);

  Face::m128fAddPS(xmm0, xmm0, xmm2);

  // Shuffle to ARGB (We have RGBA at the moment).
  Face::m128fShuffle<0, 3, 2, 1>(xmm0, xmm0, xmm0);

  Face::m128fStore16uLoHi(dst, xmm0);
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void ColorMatrix_init_SSE(void)
{
  // --------------------------------------------------------------------------
  // [Funcs]
  // --------------------------------------------------------------------------

  _api.colormatrix_addMatrix = ColorMatrix_addMatrix_SSE;
  _api.colormatrix_subtractMatrix = ColorMatrix_subtractMatrix_SSE;
  _api.colormatrix_mapArgbF = ColorMatrix_mapArgbF_SSE;
}

} // Fog namespace
