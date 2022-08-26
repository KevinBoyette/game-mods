
#include <MHW/memory.h>

#include "HUDHookHelper.h"

namespace plugin {

  using MHW::readMem;
  using MHW::writeMem;

  void HUDHookHelper::writeHUD(long long* hudStruct) {
    auto isActive = *(bool*)(hudStruct + 0x520);
    if (wasActive ^ isActive) {
      wasActive = isActive;
      long long hud;
      auto hudAddr = readMem(addresses.save_data, hud_settings, hud);
      auto mask = *reinterpret_cast<long long*>(toggles);

      hud &= ~mask;
      if (!isActive) {
        hud |= mask;
      }
      writeMem(addresses.save_data, hud_settings, hud);
    }
  }
}  // namespace plugin
