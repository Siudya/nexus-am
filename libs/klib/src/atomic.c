#include "klib.h"
#include <klib-macros.h>

uint64_t compare_and_swap(volatile uint64_t* addr, uint64_t old_val, uint64_t new_val) {
  uint64_t check = 0;
  uint64_t value = 0;
  asm volatile (
    "lr.d %[value], (%[addr]);"
    : [value]"=r"(value)
    : [addr]"p"(addr)
  );
  if (value != old_val) return 1;
  asm volatile (
    "sc.d %[check], %[write], (%[addr]);"
    : [check]"=r"(check)
    : [write]"r"(new_val), [addr]"p"(addr)
  );
  return check;
}

uint64_t atomic_add(volatile uint64_t *addr, uint64_t adder) {
  intptr_t result;
  asm volatile(
    "amoadd.d %0, %1, (%2);"
    : "=r"(result)
    : "r"(adder), "r"(addr)
  );
  return result;
}

uint64_t atomic_swap(volatile uint64_t *addr, uint64_t swapper) {
  intptr_t result;
  asm volatile(
    "amoswap.d %0, %1, (%2);"
    : "=r"(result)
    : "r"(swapper), "r"(addr)
  );
  return result;
}

void lock(volatile uint64_t *addr) {
  asm volatile("csrci mstatus, 0x8");
  while(compare_and_swap(addr, 0, 1));
}

void release(volatile uint64_t *addr) {
  *addr = 0;
  asm volatile("fence");
  asm volatile("csrsi mstatus, 0x8");
}

void barrier(uint64_t threads) {
  static volatile uint64_t barrier_var = 0;
  static volatile uint64_t flipper = 0;
  uint64_t iam;
  asm volatile(
    "csrr %0, mhartid;"
    : "=r"(iam)
  );
  uint8_t main_thread = 0 == iam;

  if(main_thread) {
    atomic_swap(&flipper, 1);
    while((threads - 1) != barrier_var);
    atomic_swap(&barrier_var, 0);
    atomic_swap(&flipper, 0);
    while((threads - 1) != barrier_var);
  } else {
    while(flipper == 0);
    atomic_add(&barrier_var, 1);
    while(flipper == 1);
    atomic_add(&barrier_var, 1);
  }
}