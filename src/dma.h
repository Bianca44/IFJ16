#ifndef DMA_H
#define DMA_H

typedef struct memory_ptr {
    void *memory_data_ptr;
    void *memory_data_info_ptr;
    struct memory_ptr *next;
} memory_ptr_t;

memory_ptr_t *memory_ptr_list;

void *dma_malloc(size_t size);
void *dma_calloc(size_t num, size_t size);
void dma_free_all();
void dma_free(void *ptr);
void *dma_realloc(void *ptr, size_t size);

#endif				// DMA_H
