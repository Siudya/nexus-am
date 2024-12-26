#ifndef __LINKNAN_PLATFORM_H__
#define __LINKNAN_PLATFORM_H__

#include "stdint.h"

#define READ_U8(addr)        (*((volatile uint8_t *)(addr)))
#define WRITE_U8(addr, data) (*((volatile uint8_t *)(addr)) = (data))

#define READ_U16(addr)        (*((volatile uint16_t *)(addr)))
#define WRITE_U16(addr, data) (*((volatile uint16_t *)(addr)) = (data))

#define READ_U32(addr)        (*((volatile uint32_t *)(addr)))
#define WRITE_U32(addr, data) (*((volatile uint32_t *)(addr)) = (data))

#define READ_U64(addr)        (*((volatile uint64_t *)(addr)))
#define WRITE_U64(addr, data) (*((volatile uint64_t *)(addr)) = (data))

#define CPU_SPACE(x) (0x0000000000L | ((x & 0x1f) << 20))
#define GET_FIELD(x, off, size) ((x >> off) & ((0x1 << size) - 1))

#define PWR_ON 2
#define PWR_RET 1
#define PWR_OFF 0

#define PLIC_BASE_ADDR          0x3c000000UL
#define INTR_GEN_ADDR           0x40070000UL

inline void riscv_fence() {
  asm volatile("fence");
}

inline void riscv_wfi() {
  asm volatile("wfi");
}

inline uint64_t riscv_mhartid() {
  uint64_t result;
  asm volatile(
    "csrr %0, mhartid;"
    : "=r"(result)
  );
  return result;
}

void enable_core(uint8_t);
#endif