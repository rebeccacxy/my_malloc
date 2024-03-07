#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

const size_t MAX_ALLOC_SIZE = 4096 * 16;
const size_t MAX_ALLOCS = 1000;
const size_t NUM_ITERS = 10000;

void *verbose_malloc(size_t size) {
  printf("malloc %lu\n", size);
  void *old_program_break = sbrk(0);

  void *addr = malloc(size);

  void *new_program_break = sbrk(0);
  bool went_up = new_program_break > old_program_break;
  size_t diff = went_up ? (size_t)new_program_break - (size_t)old_program_break
                        : (size_t)old_program_break - (size_t)new_program_break;
  printf("Program break went %s by %lu\n", went_up ? "up" : "down", diff);
  printf("malloc-ed %p\n", addr, size);

  return addr;
}

void verbose_free(void *ptr) {
  printf("free %p\n", ptr);
  void *old_program_break = sbrk(0);

  free(ptr);

  void *new_program_break = sbrk(0);
  bool went_up = new_program_break > old_program_break;
  size_t diff = went_up ? (size_t)new_program_break - (size_t)old_program_break
                        : (size_t)old_program_break - (size_t)new_program_break;
  printf("Program break went %s by %lu\n", went_up ? "up" : "down", diff);
}

void basic_test() {
  printf("Basic test\n");
  void *start = sbrk(0);
  printf("Starting break: %p\n", start);
  void *p = verbose_malloc(100);
  verbose_free(p);
  printf("Ending break: %p.\n", sbrk(0));
  printf("Net difference: %lu\n",  (size_t)sbrk(0) - (size_t)start);
}

void multiple_malloc_calls_test() {
  for (size_t i = 0; i < NUM_ITERS; ++i) {
    void *ptr = verbose_malloc(random() % MAX_ALLOC_SIZE + 1);
    free(ptr);
  }
}


int main() {
  basic_test();
  // multiple_malloc_calls_test();
}
