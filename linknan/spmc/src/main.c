#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include "spmc.h"
#include "linknan.h"

volatile uint16_t global_buffer[NUM_CORES];

int main() {
  uint64_t iam = riscv_mhartid();
  printf("hart %lu boot\n", iam);
  for(int i = 1; i< NUM_CORES; i++) enable_core(i);
  global_buffer[iam] = 0;
  riscv_fence();
  barrier(NUM_CORES);
  
  if(0 == iam) printf("[INFO]: Boot barrier is passed, start spmc tests ...\n");
  uint8_t local_buffer[NUM_CORES];
  for(int i = 0; i < NUM_CORES; i++) local_buffer[i] = 0;

  for(int i = 0; i < ITERATION; i++) {
    ++global_buffer[iam];
    riscv_fence();
    for(int j = 0; j < NUM_CORES; j++) {
      if(local_buffer[j] > global_buffer[j]) {
        printf("[ERROR]: Consumer %lu check failed, producer is %lu, local value is %d, get %d\n", iam, j, local_buffer[j], global_buffer[j]);
        return 1;
      }
    }
  }

  while(iam > 0) riscv_wfi();
  return 0;
}