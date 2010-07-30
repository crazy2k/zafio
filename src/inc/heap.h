#ifndef __SLAB_H__

#define __SLAB_H__

#include "../inc/types.h"
#include "../inc/vmm.h"

#define CACHES_NUM 8
#define CACHE_TAG_LENGTH 3 // log2(CACHES_NUM)
#define CACHE_TAG_MASK ((unsigned long) 0x7) 

#define MIN_BUCKET_SIZE 32

#define DATA_TO_BUCKET(bucket_ptr) ((cache_bucket_t*) ((void*)(bucket_ptr) - sizeof(unsigned long)) )
#define BUCKET_TO_DATA(bucket_ptr) ((void *) (bucket_ptr)->data)
#define BUCKET_DATA_SIZE(cache_ptr) ((cache_ptr)->bucket_size - sizeof(unsigned long)) 
#define CACHE_TYPE_TAG(cache_ptr) ((cache_ptr) - cache_lists)

typedef struct cache_bucket_t cache_bucket_t;

struct cache_bucket_t {
    union {
        cache_bucket_t* next;
        struct {
            unsigned long type_tag:CACHE_TAG_LENGTH;
        };
    };
    
    //Cada bucket tiene al menos 32 bytes en total, pero podrian ser más
    char data[MIN_BUCKET_SIZE - sizeof(unsigned long)];
}; 

typedef struct {
    size_t bucket_size;
    cache_bucket_t* buckets;
} type_cache_t;


void heap_configure_type(size_t size, unsigned preallocate);
    
void* kmalloc(size_t size);
void kfree(void *data);
void* stacked_malloc(size_t size);

#endif

