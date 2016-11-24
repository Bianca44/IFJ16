#include <stdio.h>
#include <stdlib.h>
#include "dma.h"

void * dma_malloc(size_t size) {
        void * ptr = malloc(size);
        if (ptr == NULL) return NULL;
        memory_ptr_t* new_item = malloc(sizeof(memory_ptr_t));

        new_item->memory_data_ptr = ptr;
        new_item->memory_data_info_ptr = new_item;
        new_item->next = memory_ptr_list;
        memory_ptr_list = new_item;
        return ptr;
}

void* dma_calloc (size_t num, size_t size) {
        void * ptr = calloc(num, size);
        if (ptr == NULL) return NULL;
        memory_ptr_t* new_item = malloc(sizeof(memory_ptr_t));

        new_item->memory_data_ptr = ptr;
        new_item->memory_data_info_ptr = new_item;
        new_item->next = memory_ptr_list;
        memory_ptr_list = new_item;
        return ptr;
}

void dma_free_all() {
        memory_ptr_t * tmp =  memory_ptr_list;
        memory_ptr_t * next = NULL;
        while (tmp != NULL) {
                free(tmp->memory_data_ptr);
                tmp->memory_data_ptr = NULL;
                next = tmp->next;
                free(tmp);
                tmp = next;
        }

}

void dma_free(void *ptr) {
        if (ptr == NULL) return;
        memory_ptr_t * tmp =  memory_ptr_list;
        memory_ptr_t * prev = NULL;
        memory_ptr_t * next = NULL;
        while (tmp != NULL) {
                if (tmp->memory_data_ptr == ptr) {
                        free(tmp->memory_data_ptr);
                        tmp->memory_data_ptr = NULL;
                        next = tmp->next;
                        free(tmp);
                        tmp = NULL;
                        if (prev == NULL) {
                                memory_ptr_list = next;
                        } else {
                                prev->next = next;
                        }
                        break;
                }
                prev = tmp;
                tmp = tmp->next;
        }

}

void * dma_realloc(void *ptr, size_t size) {
        if (ptr == NULL) return NULL;
        void * new_ptr = realloc(ptr, size);
        if (new_ptr  == NULL) return NULL;
        memory_ptr_t * tmp =  memory_ptr_list;

        while (tmp != NULL) {
                if (tmp->memory_data_ptr == ptr) {
                        tmp->memory_data_ptr = new_ptr;
                        break;
                }
                tmp = tmp->next;
        }
        return new_ptr;
}

int main() {

        void * a = dma_malloc(5);
        void * b = dma_malloc(5);
        void * x = dma_malloc(5);
        void * ptr = dma_calloc(5, 2);
        int i = 0;
        while (i < 100) {
                dma_malloc(i);
                i++;
        }

        //printf("%p\n%p\n%p\n%p\n", a,b,x,ptr);
        b = dma_realloc(b, 1000000000);
        //printf("new %p\n", ptr);
        dma_free(b);
        memory_ptr_t * pom =  memory_ptr_list;

        while (pom != NULL) {
                printf("Pointer adress (mem) %p\n", pom->memory_data_ptr);
                printf("Pointer adress (info) %p\n", pom->memory_data_info_ptr);
                pom = pom->next;
        }

        dma_free_all();



        return 0;
}
