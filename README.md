# my_malloc

`malloc` is a Standard C Library function that allocates memory chunks. Here I implement `malloc` for fun.


## Implementation

Memory is allocated in blocks. Each block contains metadata followed by data. 
Metadata stored includes a pointer to the next chunk, pointer to the previous chunk, a flag to mark free chunks, and size of the data.

We will use the system calls `sbrk` and `brk` to implement `malloc`.
```C++
int brk(const void *addr);
void *sbrk(intptr_t incr);
```
`brk` places the break at the given address `addr`.
`sbrk` moves the break by the given increment in bytes.

### First Fit malloc
In a nutshell, we use the first fit algorithm:  Traverse the chunks list and stop when we find a free block with enough space for the requested allocation.

We have two lists that keep track of allocated and freed memory chunks: A chunk list that contains the allocated memory chunks, and a freelist that contains freed but valid chunks that can't be returned to the OS as they reside at an address below some occupied block. Malloc will find the first sufficiently large free chunk, and return its address if it exists. But if there's no fitting chunk, we need to extend the heap (especially at the beginning).

When `free` is called on a chunk, it is marked as free and added to the freelist. If it is at the end of the heap, we traverse backwards to find the smallest address that we can set the program break to. Prune the chunk list, traverse the freelist to remove chunks that reside beyond this new address and use `brk` to set the new program break.  

