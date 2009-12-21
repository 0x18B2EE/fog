// [Fog/Graphics Library - C++ API]
//
// [Licence]
// MIT, See COPYING file in package

//----------------------------------------------------------------------------
// Anti-Grain Geometry - Version 2.4
// Copyright (C) 2002-2005 Maxim Shemanarev (http://www.antigrain.com)
//
// Permission to copy, use, modify, sell and distribute this software
// is granted provided this copyright notice appears in all copies.
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//
//----------------------------------------------------------------------------
// Contact: mcseem@antigrain.com
//          mcseemagg@yahoo.com
//          http://www.antigrain.com
//----------------------------------------------------------------------------

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/CpuInfo.h>
#include <Fog/Core/Math.h>
#include <Fog/Graphics/Matrix.h>
#include <Fog/Graphics/Path.h>
#include <Fog/Graphics/PathUtil.h>

namespace Fog {
namespace PathUtil {

// ============================================================================
// [Fog::PathUtil::Function Map]
// ============================================================================

FunctionMap fm;

// ============================================================================
// [Fog::PathUtil::Transformations]
// ============================================================================

static void FOG_FASTCALL transformVertex(PathVertex* data, sysuint_t length, const Matrix* matrix)
{
  for (sysuint_t i = length; i; i--, data++)
  {
    if (data->cmd.isVertex())
    {
      double nx = data->x * matrix->sx  + data->y * matrix->shx + matrix->tx;
      double ny = data->x * matrix->shy + data->y * matrix->sy  + matrix->ty;

      data->x = nx;
      data->y = ny;
    }
  }
}

static void FOG_FASTCALL transformVertex2(PathVertex* FOG_RESTRICT dst, const PathVertex* FOG_RESTRICT src, sysuint_t length, const Matrix* matrix)
{
  for (sysuint_t i = length; i; i--, dst++, src++)
  {
    PathCmd cmd = src->cmd;

    dst->cmd = cmd;
    if (cmd.isVertex())
    {
      double nx = src->x * matrix->sx  + src->y * matrix->shx + matrix->tx;
      double ny = src->x * matrix->shy + src->y * matrix->sy  + matrix->ty;

      dst->x = nx;
      dst->y = ny;
    }
    else
    {
      dst->x = src->x;
      dst->y = src->y;
    }
  }
}

// ============================================================================
// [Fog::PathUtil::Curve Approximation]
// ============================================================================

#define VERTEX_INITIAL_SIZE 256
#define ADD_VERTEX(_cmd, _x, _y) { v->x = _x; v->y = _y; v->cmd = _cmd; v++; }

enum { APPROXIMATE_CURVE3_RECURSION_LIMIT = 32 };
enum { APPROXIMATE_CURVE4_RECURSION_LIMIT = 32 };

static err_t FOG_FASTCALL approximateCurve3(
  Path& dst,
  double x1, double y1,
  double x2, double y2,
  double x3, double y3,
  double approximationScale,
  double angleTolerance = 0.0)
{
  double distanceToleranceSquare = 0.5 / approximationScale;
  distanceToleranceSquare *= distanceToleranceSquare;

  sysuint_t level = 0;
  ApproximateCurve3Data stack[APPROXIMATE_CURVE3_RECURSION_LIMIT];

  PathVertex* v;
  PathVertex* vend;

realloc:
  v = dst._add(VERTEX_INITIAL_SIZE);
  if (!v) return ERR_RT_OUT_OF_MEMORY;
  vend = v + VERTEX_INITIAL_SIZE - 1;

  for (;;)
  {
    // Realloc if needed.
    if (v >= vend) { dst._d->length = (sysuint_t)(v - dst._d->data); goto realloc; }

    // Calculate all the mid-points of the line segments.
    double x12   = (x1 + x2) / 2.0;
    double y12   = (y1 + y2) / 2.0;
    double x23   = (x2 + x3) / 2.0;
    double y23   = (y2 + y3) / 2.0;
    double x123  = (x12 + x23) / 2.0;
    double y123  = (y12 + y23) / 2.0;

    double dx = x3-x1;
    double dy = y3-y1;
    double d = fabs(((x2 - x3) * dy - (y2 - y3) * dx));
    double da;

    if (d > CURVE_COLLINEARITY_EPSILON)
    {
      // Regular case.
      if (d * d <= distanceToleranceSquare * (dx*dx + dy*dy))
      {
        // If the curvature doesn't exceed the distanceTolerance value
        // we tend to finish subdivisions.
        if (angleTolerance < CURVE_ANGLE_TOLERANCE_EPSILON)
        {
          ADD_VERTEX(PATH_CMD_LINE_TO, x123, y123);
          goto ret;
        }

        // Angle & Cusp Condition.
        da = fabs(atan2(y3 - y2, x3 - x2) - atan2(y2 - y1, x2 - x1));
        if (da >= M_PI) da = 2.0 * M_PI - da;

        if (da < angleTolerance)
        {
          // Finally we can stop the recursion.
          ADD_VERTEX(PATH_CMD_LINE_TO, x123, y123);
          goto ret;
        }
      }
    }
    else
    {
      // Collinear case.
      da = dx*dx + dy*dy;
      if (da == 0)
      {
        d = calcSqDistance(x1, y1, x2, y2);
      }
      else
      {
        d = ((x2 - x1)*dx + (y2 - y1)*dy) / da;

        if (d > 0 && d < 1)
        {
          // Simple collinear case, 1---2---3.
          // We can leave just two endpoints.
          goto ret;
        }

        if (d <= 0)
          d = calcSqDistance(x2, y2, x1, y1);
        else if (d >= 1)
          d = calcSqDistance(x2, y2, x3, y3);
        else
          d = calcSqDistance(x2, y2, x1 + d*dx, y1 + d*dy);
      }
      if (d < distanceToleranceSquare)
      {
        ADD_VERTEX(PATH_CMD_LINE_TO, x2, y2);
        goto ret;
      }
    }

    // Continue subdivision.
    //
    // Original code from antigrain was:
    //   recursive_bezier(x1, y1, x12, y12, x123, y123, level + 1);
    //   recursive_bezier(x123, y123, x23, y23, x3, y3, level + 1);
    //
    // First recursive subdivision will be set into x1, y1, x2, y2, x3, y3,
    // second subdivision will be added into stack.

    if (level < APPROXIMATE_CURVE3_RECURSION_LIMIT)
    {
      stack[level].x1 = x123;
      stack[level].y1 = y123;
      stack[level].x2 = x23;
      stack[level].y2 = y23;
      stack[level].x3 = x3;
      stack[level].y3 = y3;
      level++;

      x2 = x12;
      y2 = y12;
      x3 = x123;
      y3 = y123;

      continue;
    }

ret:
    if (level == 0) break;

    level--;
    x1 = stack[level].x1;
    y1 = stack[level].y1;
    x2 = stack[level].x2;
    y2 = stack[level].y2;
    x3 = stack[level].x3;
    y3 = stack[level].y3;
  }

  // Add end point.
  ADD_VERTEX(PATH_CMD_LINE_TO, x3, y3);

  dst._d->length = (sysuint_t)(v - dst._d->data);
  FOG_ASSERT(dst._d->capacity >= dst._d->length);

  return ERR_OK;
}

static err_t FOG_FASTCALL approximateCurve4(
  Path& dst,
  double x1, double y1,
  double x2, double y2,
  double x3, double y3,
  double x4, double y4,
  double approximationScale,
  double angleTolerance = 0.0,
  double cuspLimit = 0.0)
{
  double distanceToleranceSquare = 0.5 / approximationScale;
  distanceToleranceSquare *= distanceToleranceSquare;

  sysuint_t level = 0;
  ApproximateCurve4Data stack[APPROXIMATE_CURVE4_RECURSION_LIMIT];

  PathVertex* v;
  PathVertex* vend;

realloc:
  v = dst._add(VERTEX_INITIAL_SIZE);
  if (!v) return ERR_RT_OUT_OF_MEMORY;
  vend = v + VERTEX_INITIAL_SIZE - 2;

  for (;;)
  {
    // Realloc if needed.
    if (v >= vend) { dst._d->length = (sysuint_t)(v - dst._d->data); goto realloc; }

    // Calculate all the mid-points of the line segments.
    double x12   = (x1 + x2) / 2.0;
    double y12   = (y1 + y2) / 2.0;
    double x23   = (x2 + x3) / 2.0;
    double y23   = (y2 + y3) / 2.0;
    double x34   = (x3 + x4) / 2.0;
    double y34   = (y3 + y4) / 2.0;
    double x123  = (x12 + x23) / 2.0;
    double y123  = (y12 + y23) / 2.0;
    double x234  = (x23 + x34) / 2.0;
    double y234  = (y23 + y34) / 2.0;
    double x1234 = (x123 + x234) / 2.0;
    double y1234 = (y123 + y234) / 2.0;

    // Try to approximate the full cubic curve by a single straight line.
    double dx = x4 - x1;
    double dy = y4 - y1;

    double d2 = fabs(((x2 - x4) * dy - (y2 - y4) * dx));
    double d3 = fabs(((x3 - x4) * dy - (y3 - y4) * dx));
    double da1, da2, k;

    switch ((int(d2 > CURVE_COLLINEARITY_EPSILON) << 1) +
             int(d3 > CURVE_COLLINEARITY_EPSILON))
    {
      // All collinear OR p1 == p4.
      case 0:
        k = dx*dx + dy*dy;
        if (k == 0)
        {
          d2 = calcSqDistance(x1, y1, x2, y2);
          d3 = calcSqDistance(x4, y4, x3, y3);
        }
        else
        {
          k   = 1 / k;
          da1 = x2 - x1;
          da2 = y2 - y1;
          d2  = k * (da1 * dx + da2 * dy);
          da1 = x3 - x1;
          da2 = y3 - y1;
          d3  = k * (da1 * dx + da2 * dy);

          if (d2 > 0 && d2 < 1 && d3 > 0 && d3 < 1)
          {
            // Simple collinear case, 1---2---3---4.
            // We can leave just two endpoints.
            goto ret;
          }

          if (d2 <= 0)
            d2 = calcSqDistance(x2, y2, x1, y1);
          else if (d2 >= 1)
            d2 = calcSqDistance(x2, y2, x4, y4);
          else
            d2 = calcSqDistance(x2, y2, x1 + d2*dx, y1 + d2*dy);

          if (d3 <= 0)
            d3 = calcSqDistance(x3, y3, x1, y1);
          else if (d3 >= 1)
            d3 = calcSqDistance(x3, y3, x4, y4);
          else
            d3 = calcSqDistance(x3, y3, x1 + d3*dx, y1 + d3*dy);
        }

        if (d2 > d3)
        {
          if (d2 < distanceToleranceSquare)
          {
            ADD_VERTEX(PATH_CMD_LINE_TO, x2, y2);
            goto ret;
          }
        }
        else
        {
          if (d3 < distanceToleranceSquare)
          {
            ADD_VERTEX(PATH_CMD_LINE_TO, x3, y3);
            goto ret;
          }
        }
        break;

    // p1, p2, p4 are collinear, p3 is significant.
    case 1:
      if (d3 * d3 <= distanceToleranceSquare * (dx*dx + dy*dy))
      {
        if (angleTolerance < CURVE_ANGLE_TOLERANCE_EPSILON)
        {
          ADD_VERTEX(PATH_CMD_LINE_TO, x23, y23);
          goto ret;
        }

        // Angle Condition.
        da1 = fabs(atan2(y4 - y3, x4 - x3) - atan2(y3 - y2, x3 - x2));
        if (da1 >= M_PI) da1 = 2.0 * M_PI - da1;

        if (da1 < angleTolerance)
        {
          ADD_VERTEX(PATH_CMD_LINE_TO, x2, y2);
          ADD_VERTEX(PATH_CMD_LINE_TO, x3, y3);
          goto ret;
        }

        if (cuspLimit != 0.0)
        {
          if (da1 > cuspLimit)
          {
            ADD_VERTEX(PATH_CMD_LINE_TO, x3, y3);
            goto ret;
          }
        }
      }
      break;

    // p1, p3, p4 are collinear, p2 is significant.
    case 2:
      if (d2 * d2 <= distanceToleranceSquare * (dx*dx + dy*dy))
      {
        if (angleTolerance < CURVE_ANGLE_TOLERANCE_EPSILON)
        {
          ADD_VERTEX(PATH_CMD_LINE_TO, x23, y23);
          goto ret;
        }

        // Angle Condition.
        da1 = fabs(atan2(y3 - y2, x3 - x2) - atan2(y2 - y1, x2 - x1));
        if (da1 >= M_PI) da1 = 2.0 * M_PI - da1;

        if (da1 < angleTolerance)
        {
          ADD_VERTEX(PATH_CMD_LINE_TO, x2, y2);
          ADD_VERTEX(PATH_CMD_LINE_TO, x3, y3);
          goto ret;
        }

        if (cuspLimit != 0.0)
        {
          if (da1 > cuspLimit)
          {
            ADD_VERTEX(PATH_CMD_LINE_TO, x3, y3);
            goto ret;
          }
        }
      }
      break;

    // Regular case.
    case 3:
      if ((d2 + d3)*(d2 + d3) <= distanceToleranceSquare * (dx*dx + dy*dy))
      {
        // If the curvature doesn't exceed the distance_tolerance value
        // we tend to finish subdivisions.
        if (angleTolerance < CURVE_ANGLE_TOLERANCE_EPSILON)
        {
          ADD_VERTEX(PATH_CMD_LINE_TO, x23, y23);
          goto ret;
        }

        // Angle & Cusp Condition.
        k   = atan2(y3 - y2, x3 - x2);
        da1 = fabs(k - atan2(y2 - y1, x2 - x1));
        da2 = fabs(atan2(y4 - y3, x4 - x3) - k);
        if (da1 >= M_PI) da1 = 2.0 * M_PI - da1;
        if (da2 >= M_PI) da2 = 2.0 * M_PI - da2;

        if (da1 + da2 < angleTolerance)
        {
          // Finally we can stop the recursion.
          ADD_VERTEX(PATH_CMD_LINE_TO, x23, y23);
          goto ret;
        }

        if (cuspLimit != 0.0)
        {
          if (da1 > cuspLimit)
          {
            ADD_VERTEX(PATH_CMD_LINE_TO, x2, y2);
            goto ret;
          }

          if (da2 > cuspLimit)
          {
            ADD_VERTEX(PATH_CMD_LINE_TO, x3, y3);
            goto ret;
          }
        }
      }
      break;
    }

    // Continue subdivision.
    //
    // Original antigrain code:
    //   recursive_bezier(x1, y1, x12, y12, x123, y123, x1234, y1234, level + 1);
    //   recursive_bezier(x1234, y1234, x234, y234, x34, y34, x4, y4, level + 1);
    //
    // First recursive subdivision will be set into x1, y1, x2, y2, x3, y3,
    // second subdivision will be added into stack.
    if (level < APPROXIMATE_CURVE4_RECURSION_LIMIT)
    {
      stack[level].x1 = x1234;
      stack[level].y1 = y1234;
      stack[level].x2 = x234;
      stack[level].y2 = y234;
      stack[level].x3 = x34;
      stack[level].y3 = y34;
      stack[level].x4 = x4;
      stack[level].y4 = y4;
      level++;

      x2 = x12;
      y2 = y12;
      x3 = x123;
      y3 = y123;
      x4 = x1234;
      y4 = y1234;

      continue;
    }

ret:
    if (level == 0) break;

    level--;
    x1 = stack[level].x1;
    y1 = stack[level].y1;
    x2 = stack[level].x2;
    y2 = stack[level].y2;
    x3 = stack[level].x3;
    y3 = stack[level].y3;
    x4 = stack[level].x4;
    y4 = stack[level].y4;
  }

  // Add end point.
  ADD_VERTEX(PATH_CMD_LINE_TO, x4, y4);

  dst._d->length = (sysuint_t)(v - dst._d->data);
  FOG_ASSERT(dst._d->capacity >= dst._d->length);

  return ERR_OK;
}

#undef ADD_VERTEX

} // PathUtil namespace
} // Fog namespace

// ============================================================================
// [Library Initializers]
// ============================================================================

#if defined(FOG_ARCH_X86) || defined(FOG_ARCH_X86_64)
FOG_INIT_EXTERN void fog_pathutil_init_sse2(void);
#endif // FOG_ARCH_X86 || FOG_ARCH_X86_64

FOG_INIT_DECLARE err_t fog_pathutil_init(void)
{
  using namespace Fog;

  // Install C optimized code (default).
  PathUtil::fm.transformVertex = PathUtil::transformVertex;
  PathUtil::fm.transformVertex2 = PathUtil::transformVertex2;
  PathUtil::fm.approximateCurve3 = PathUtil::approximateCurve3;
  PathUtil::fm.approximateCurve4 = PathUtil::approximateCurve4;

  // Install SSE2 optimized code if supported.
#if defined(FOG_ARCH_X86) || defined(FOG_ARCH_X86_64)
  if (cpuInfo->hasFeature(CpuInfo::FEATURE_SSE2)) fog_pathutil_init_sse2();
#endif // FOG_ARCH_X86 || FOG_ARCH_X86_64

  return ERR_OK;
}

FOG_INIT_DECLARE void fog_pathutil_shutdown(void)
{
}