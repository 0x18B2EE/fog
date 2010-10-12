// [Fog-Graphics]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_PATTERN_H
#define _FOG_GRAPHICS_PATTERN_H

// [Dependencies]
#include <Fog/Core/Static.h>
#include <Fog/Graphics/Color.h>
#include <Fog/Graphics/Constants.h>
#include <Fog/Graphics/Geometry.h>
#include <Fog/Graphics/Gradient.h>
#include <Fog/Graphics/Image.h>
#include <Fog/Graphics/Transform.h>

namespace Fog {

//! @addtogroup Fog_Graphics_Painting
//! @{

// ============================================================================
// [Fog::PatternData]
// ============================================================================

//! Pattern data.
struct FOG_API PatternData
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  PatternData();
  PatternData(const PatternData& other);
  ~PatternData();

  // --------------------------------------------------------------------------
  // [Ref / Deref]
  // --------------------------------------------------------------------------

  FOG_INLINE PatternData* ref() const
  {
    refCount.inc();
    return const_cast<PatternData*>(this);
  }

  FOG_INLINE void deref()
  {
    if (refCount.deref()) fog_delete(this);
  }

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  void deleteResources();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Reference count.
  mutable Atomic<sysuint_t> refCount;

  //! @brief pattern type, see @c PATTERN_TYPE enum.
  uint32_t type;
  //! @brief Pattern spread, see @c PATTERN_SPREAD enum.
  uint32_t spread;

  //! @brief Pattern transformation matrix.
  TransformD transform;

  //! @internal
  struct GradientData
  {
    //! @brief Start and end points. These values have different meaning for
    //! each pattern type:
    //! - Linear gradient - points[0] is start point and points[1] is end point.
    //! - Radial gradient - points[0] is center point, points[1] is focal point.
    //! - Conical gradient - points[0] is center point, points[1] is end point (for angle).
    PointD points[2];
    //! @brief Used only for PATTERN_RADIAL_GRADIENT - circle radius.
    double radius;
  };

  //! @internal
  struct TextureData
  {
    RectI area;
  };

  //! @internal
  //!
  //! @brief Embedded objects in pattern, this can be solid color, raster
  //! texture data and gradient stops.
  union ObjectInst
  {
    Static< ArgbI > argb;
    Static< Image > texture;
    Static< List<ArgbStop> > stops;
  } obj;

  //! @internal
  //!
  //! @brief Data for all pattern types.
  union ObjectData
  {
    Static< TextureData > texture;
    Static< GradientData > gradient;
  } data;
};

// ============================================================================
// [Fog::Pattern]
// ============================================================================

//! @brief Pattern can be used to define stroke or fill source when painting.
//!
//! Pattern is class that can be used to define these types of sources:
//! - Null - NOP (no source, no painting...).
//! - Solid color - Everything is painted by solid color.
//! - Texture - Raster texture is used as a source.
//! - Linear gradient - Linear gradient defined between two points.
//! - Radial gradient - Radial gradient defined by one circle, radius and focal point.
//! - Conical gradient - Conical gradient - atan() function is used.
struct FOG_API Pattern
{
  static Static<PatternData> _dnull;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  Pattern();
  Pattern(const Pattern& other);
  explicit Pattern(const ArgbI& argb);
  FOG_INLINE explicit Pattern(PatternData* d) : _d(d) {}
  ~Pattern();

  // --------------------------------------------------------------------------
  // [Implicit Sharing]
  // --------------------------------------------------------------------------

  FOG_INLINE sysuint_t refCount() const { return _d->refCount.get(); }
  FOG_INLINE sysuint_t isDetached() const { return _d->refCount.get() == 1; }

  FOG_INLINE err_t detach() { return (_d->refCount.get() > 1) ? _detach() : ERR_OK; }
  err_t _detach();

  void free();

  // --------------------------------------------------------------------------
  // [Type]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getType() const { return _d->type; }
  err_t setType(uint32_t type);

  FOG_INLINE bool isNull() const { return _d->type == PATTERN_NONE; }
  FOG_INLINE bool isSolid() const { return _d->type == PATTERN_SOLID; }
  FOG_INLINE bool isTexture() const { return _d->type == PATTERN_TEXTURE; }
  FOG_INLINE bool isGradient() const { return (_d->type & PATTERN_IS_GRADIENT) == PATTERN_IS_GRADIENT; }
  FOG_INLINE bool isLinearGradient() const { return _d->type == PATTERN_LINEAR_GRADIENT; }
  FOG_INLINE bool isRadialGradient() const { return _d->type == PATTERN_RADIAL_GRADIENT; }
  FOG_INLINE bool isConicalGradient() const { return _d->type == PATTERN_CONICAL_GRADIENT; }

  // --------------------------------------------------------------------------
  // [Null]
  // --------------------------------------------------------------------------

  void reset();

  // --------------------------------------------------------------------------
  // [Spread]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getSpread() const { return _d->spread; }
  err_t setSpread(uint32_t spread);

  // --------------------------------------------------------------------------
  // [Matrix]
  // --------------------------------------------------------------------------

  FOG_INLINE TransformD getTransform() const { return _d->transform; }
  err_t setTransform(const TransformF& transform);
  err_t setTransform(const TransformD& transform);
  err_t resetTransform();

  // TODO: Use TRANSFORM_OP and merge to one exported function.
  err_t translate(float x, float y, uint32_t order = MATRIX_ORDER_PREPEND);
  err_t translate(double x, double y, uint32_t order = MATRIX_ORDER_PREPEND);
  err_t rotate(float a, uint32_t order = MATRIX_ORDER_PREPEND);
  err_t rotate(double a, uint32_t order = MATRIX_ORDER_PREPEND);
  err_t scale(float x, float y, uint32_t order = MATRIX_ORDER_PREPEND);
  err_t scale(double x, double y, uint32_t order = MATRIX_ORDER_PREPEND);
  err_t skew(float x, float y, uint32_t order = MATRIX_ORDER_PREPEND);
  err_t skew(double x, double y, uint32_t order = MATRIX_ORDER_PREPEND);
  err_t transform(const TransformF& m, uint32_t order = MATRIX_ORDER_PREPEND);
  err_t transform(const TransformD& m, uint32_t order = MATRIX_ORDER_PREPEND);

  // --------------------------------------------------------------------------
  // [Solid]
  // --------------------------------------------------------------------------

  //! @brief Get pattern color (for @c PATTERN_SOLID type).
  //!
  //! If the pattern is not @c PATTERN_SOLID type, the ArgbI(0, 0, 0, 0) color is
  //! returned.
  ArgbI getColor() const;

  //! @brief Set pattern type to @c PATTERN_SOLID and its color to @a argb.
  err_t setColor(const ArgbI& argb);

  // --------------------------------------------------------------------------
  // [Texture]
  // --------------------------------------------------------------------------

  //! @brief Get pattern texture (for @c PATTERN_TEXTURE type).
  Image getTexture() const;

  //! @brief Get texture area (for @c PATTERN_TEXTURE type).
  RectI getTextureArea() const;

  //! @brief Set pattern type to @c PATTERN_TEXTURE and the texture to @a texture.
  //!
  //! @return
  //! - @c ERR_OK - If function succeed.
  //! - @c ERR_RT_INVALID_OBJECT - If pattern is not @c PATTERN_RADIAL_GRADIENT.
  //! - @c ERR_RT_OUT_OF_MEMORY - If memory allocation failed.
  err_t setTexture(const Image& texture);
  //! @overload
  err_t setTexture(const Image& texture, const RectI& textureArea);

  // --------------------------------------------------------------------------
  // [Gradient]
  // --------------------------------------------------------------------------

  Gradient getGradient() const;
  err_t setGradient(const Gradient& gradient);

  PointD getStartPoint() const;
  PointD getEndPoint() const;

  err_t setStartPoint(const PointI& pt);
  err_t setStartPoint(const PointF& pt);
  err_t setStartPoint(const PointD& pt);

  err_t setEndPoint(const PointI& pt);
  err_t setEndPoint(const PointF& pt);
  err_t setEndPoint(const PointD& pt);

  err_t setPoints(const PointI& startPt, const PointI& endPt);
  err_t setPoints(const PointF& startPt, const PointF& endPt);
  err_t setPoints(const PointD& startPt, const PointD& endPt);

  //! @brief Get radial gradient radius (for @c PATTERN_RADIAL_GRADIENT type)
  double getRadius() const;

  //! @brief Set radial gradient radius to @a r.
  //!
  //! @return
  //! - @c ERR_OK - If function succeed.
  //! - @c ERR_RT_INVALID_OBJECT - If pattern is not @c PATTERN_RADIAL_GRADIENT.
  //! - @c ERR_RT_OUT_OF_MEMORY - If memory allocation failed.
  err_t setRadius(float r);
  //! @overload
  err_t setRadius(double r);

  //! @brief Get generic gradient argb stops (for @c PATTERN_LINEAR_GRADIENT,
  //! @c PATTERN_RADIAL_GRADIENT and @c PATTERN_CONICAL_GRADIENT types).
  List<ArgbStop> getStops() const;

  //! @brief Set generic gradient argb stops to @a stops.
  //!
  //! @return
  //! - @c ERR_OK - If function succeed.
  //! - @c ERR_RT_INVALID_OJBECT - If pattern is not @c PATTERN_LINEAR_GRADIENT,
  //!   @c PATTERN_RADIAL_GRADIENT or @c PATTERN_CONICAL_GRADIENT.
  //! - @c ERR_RT_OUT_OF_MEMORY - If memory allocation failed.
  err_t setStops(const List<ArgbStop>& stops);

  //! @brief Reset generic gradient argb stops (stops will be empty).
  //!
  //! @return
  //! - @c ERR_OK - If function succeed.
  //! - @c ERR_RT_INVALID_OBJECT - If pattern is not @c PATTERN_LINEAR_GRADIENT,
  //!   @c PATTERN_RADIAL_GRADIENT or @c PATTERN_CONICAL_GRADIENT.
  //! - @c ERR_RT_OUT_OF_MEMORY - If memory allocation failed.
  err_t resetStops();

  //! @brief Add generic gradient argb stop @a stop to stops list.
  //!
  //! @return
  //! - @c ERR_OK - If function succeed.
  //! - @c ERR_RT_INVALID_OBJECT - If pattern is not @c PATTERN_LINEAR_GRADIENT,
  //!   @c PATTERN_RADIAL_GRADIENT or @c PATTERN_CONICAL_GRADIENT.
  //! - @c ERR_RT_OUT_OF_MEMORY - If memory allocation failed.
  err_t addStop(const ArgbStop& stop);

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  Pattern& operator=(const Pattern& other);
  Pattern& operator=(const ArgbI& rgba);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  FOG_DECLARE_D(PatternData)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_GRAPHICS_PATTERN_H
