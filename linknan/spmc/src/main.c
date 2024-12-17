#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include "spmc.h"
#include "linknan.h"

volatile uint8_t __attribute__((aligned(64))) global_buffer[NUM_CORES];

uint8_t test() {
  uint64_t iam = riscv_mhartid();
  uint8_t local_buffer[NUM_CORES];
  for(int i = 0; i < NUM_CORES; i++) local_buffer[i] = 0;
  for(int i = 0; i < 255; i++) {
    ++global_buffer[iam];
    riscv_fence();
    for(int j = 0; j < NUM_CORES; j++) {
      if(local_buffer[j] > global_buffer[j]) {
        printf("[ERROR]: Consumer %lu check failed, producer is %lu, local value is %d, get %d\n", iam, j, local_buffer[j], global_buffer[j]);
        return 1;
      }
      local_buffer[j] = global_buffer[j];
    }
  }
  return 0;
}

void clean_global_buffer() {
  for(int i = 0; i< NUM_CORES; i++) global_buffer[i] = 0;
  riscv_fence();
}

int main() {
  uint64_t iam = riscv_mhartid();
  printf("hart %lu boot\n", iam);
  if(0 == iam) {
    for(int i = 1; i< NUM_CORES; i++) enable_core(i);
    clean_global_buffer();
  }
  barrier(NUM_CORES);
  
  if(0 == iam) printf("[INFO]: Boot barrier is passed, start spmc tests ...\n");
  
  for(int i = 0; i < ITERATION; i++) {
    if(test()) return 1;
    barrier(NUM_CORES);
    if(0 == iam) {
      printf("[INFO]: Iteration %d ends!\n", i);
      clean_global_buffer();
    }
    barrier(NUM_CORES);
  }
  while(iam > 0) riscv_wfi();
  return 0;
}