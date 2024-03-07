#ifndef MY_MALLOC_H
#define MY_MALLOC_H

#include <unistd.h>

void *malloc(size_t size);
void free(void *ptr);

#endif