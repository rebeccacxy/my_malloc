#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

typedef struct malloc_block {
    size_t size;
    struct malloc_block *next;
    struct malloc_block *prev;
    bool free;
} malloc_block_t;

malloc_block_t *global_head = NULL;
malloc_block_t *global_tail = NULL;
malloc_block_t *free_head = NULL;
malloc_block_t *free_tail = NULL;


// Adds a new block at the end of heap
void *extend_heap(size_t size) {
    // allocate space for metadata and data
    char *sb = sbrk(size + sizeof(malloc_block_t));
    if (sb == (void *)-1) {
        // failed to allocate
        return NULL;
    }
    void *data_ptr = sb + sizeof(malloc_block_t);
    malloc_block_t *metadata_ptr = (malloc_block_t *)sb;metadata_ptr->size = size;
    metadata_ptr->next = NULL;
    metadata_ptr->prev = global_tail;
    metadata_ptr->free = false;

    // first block
    if (global_head == NULL) {
        global_head = metadata_ptr;
    }

    global_tail = metadata_ptr;
    return data_ptr;
}

// returns a pointer to a block of memory of the given size
void *malloc(size_t size) {
    if (size <= 0) return NULL;
    if (free_head == NULL) return extend_heap(size);

    malloc_block_t *prev = free_head;
    malloc_block_t *curr = free_head->next;

    // check if head can be reused
    if (free_head->size >= size) {
        free_head->free = false;

        // update freelist
        free_head = free_head->next;
        prev->next = NULL;

        return (char *)prev + sizeof(malloc_block_t);
    }

    // try reusing any other element of the freelist
    while (curr) {
        if (curr->size >= size) {
            curr->free = false;
            // remove curr from list by making prev bypass it
            prev->next = curr->next;
            if (prev->next == NULL) {
                // new tail
                free_tail = prev;
            }

            curr->next = NULL;
            return (char*)curr + sizeof(malloc_block_t);
        }

        prev = curr;
        curr = curr->next;
    }

    // no free chunks found, extend end of the heap
    return extend_heap(size);
}

void prune_free_list(void *addr) {
     // empty list
    if (free_head == NULL) {
        return;
    }
    
    while (free_head && free_head >= addr) {
        free_head = free_head->next;
    }

    malloc_block_t *prev = free_head;
    malloc_block_t *curr = free_head->next;
    while (curr != NULL) {
        if (curr >= addr) {
            // delete prev from freelist
            prev->next = curr->next;
            if (curr == free_tail) {
                free_tail = prev;
            }
            curr = curr->next;
        } else {
            prev = curr;
            curr = curr->next;
        }
    }
}

// returns a pointer to the region after malloc_block_t
malloc_block_t *get_free_block(void *p) {
    char *tmp = p;
    return (p = tmp -= sizeof(malloc_block_t));
}

// gets the corresponding chunk, marks it as free and prunes the free list
void free(void *p) {
    malloc_block_t *block = get_free_block(p);
    block->free = true;
    // add block to the free list
    if (free_head) {
        free_tail->next = block;
    } else {
        free_head = block;
    }
    free_tail = block;

    // freeing the last block. search backwards for contiguous free blocks
    if (block == global_tail) {
        while (block->prev && block->prev->free) {
            block = block->prev;
        }

        if (block->prev == NULL) {
            malloc_block_t *new_program_break = global_head;
            global_head = NULL;
            global_tail = NULL;
            free_head = NULL;
            free_tail = NULL;
            brk(new_program_break);
            return;
        }

        malloc_block_t *new_program_break = block;
        global_tail = block->prev;
        prune_free_list(new_program_break);
        brk(new_program_break);
    }
}
