#ifndef __HEAP_H__

#define __HEAP_H__

#include "../inc/types.h"
#include "../inc/vmm.h"

#define CACHE_COUNT 8
#define MIN_BUCKET_SIZE 32
#define AVG_BUCKETS_PER_PAGE 64

#define DATA_TO_BUCKET(bucket_ptr) ((cache_bucket_t*) ((void*)(bucket_ptr) - sizeof(unsigned long)))
#define BUCKET_TO_DATA(bucket_ptr) ((void *) (bucket_ptr)->data)
#define BUCKET_DATA_SIZE(cache_ptr) ((cache_ptr)->bucket_size - sizeof(unsigned long)) 
#define CACHE_TYPE_TAG(cache_ptr) ((cache_ptr) - cache_lists)
#define CHUNK_TOTAL_BUCKETS(cache_ptr) ((PAGE_SIZE*(cache_ptr)->grow_rate) / (cache_ptr)->bucket_size)

typedef struct cache_bucket_t cache_bucket_t;

struct cache_bucket_t {
    union {
        cache_bucket_t* next;
        unsigned long type_tag;
    };
    
    //Cada bucket tiene al menos 32 bytes en total, pero podrian ser más
    char data[MIN_BUCKET_SIZE - sizeof(unsigned long)];
}; 

typedef struct {
    size_t bucket_size;
    long grow_rate;
    cache_bucket_t* buckets;
} type_cache_t;


void heap_config_type(size_t size, long pages);
void* kmalloc(size_t size);
void kfree(void *data);

#endif

