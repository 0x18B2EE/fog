// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/Core/Tools/Logger.h>
#include <Fog/G2d/Text/OpenType/OTEnum.h>
#include <Fog/G2d/Text/OpenType/OTMaxp.h>

namespace Fog {

// ============================================================================
// [Fog::OTMaxp - Init / Destroy]
// ============================================================================

static void FOG_CDECL OTMaxp_destroy(OTMaxp* self)
{
  // This results in crash in case that destroy is called twice by accident.
  self->_destroy = NULL;
}

static err_t FOG_CDECL OTMaxp_init(OTMaxp* self)
{
  // --------------------------------------------------------------------------
  // [Init]
  // --------------------------------------------------------------------------

  const uint8_t* data = self->getData();
  uint32_t dataLength = self->getDataLength();

#if defined(FOG_OT_DEBUG)
  Logger::info("Fog::OTMaxp", "init", 
    "Initializing 'hhea' table (%u bytes).", dataLength);
#endif // FOG_OT_DEBUG

  FOG_ASSERT_X(self->_tag == FOG_OT_TAG('m', 'a', 'x', 'p'),
    "Fog::OTMaxp::init() - Not a 'maxp' table.");

  self->_destroy = (OTTableDestroyFunc)OTMaxp_destroy;

  // --------------------------------------------------------------------------
  // [Header]
  // --------------------------------------------------------------------------

  const OTMaxpHeader* header = self->getHeader();

  return ERR_OK;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void OTMaxp_init(void)
{
  OTApi& api = fog_ot_api;

  // --------------------------------------------------------------------------
  // [OTMaxp]
  // --------------------------------------------------------------------------
  
  api.otmaxp_init = OTMaxp_init;
}

} // Fog namespace