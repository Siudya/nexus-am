#include <am.h>
#include "linknan.h"

void enable_pwr_ctl(uint64_t address) {
  uint32_t state = READ_U32(address + 0x4);
  uint8_t dev_mode = GET_FIELD(state, 0, 8);
  if(dev_mode != PWR_ON) {
    WRITE_U32(address + 0x0, PWR_ON);
    state = READ_U32(address + 0x4);
    dev_mode = GET_FIELD(state, 0, 8);
  }
  while(dev_mode != PWR_ON) {
    state = READ_U32(address + 0x4);
    dev_mode = GET_FIELD(state, 0, 8);
  }
}

void enable_core(uint8_t c) {
  uint64_t core_pwr_ctl = CPU_SPACE(c) + 0xA000;
  uint64_t csu_pwr_ctl = CPU_SPACE(c) + 0x11000;

  enable_pwr_ctl(csu_pwr_ctl);
  enable_pwr_ctl(core_pwr_ctl);
}