#ifndef __SLAB_H__

#define __SLAB_H__

#include "../inc/types.h"
#include "../inc/vmm.h"

#define CACHES_NUM 8
#define CACHE_TAG_LENGTH 3 // log2(CACHES_NUM)
#define CACHE_TAG_MASK ((unsigned long) 0x7) 

#define MIN_BUCKET_SIZE 32

#define TYPE_TAG(cache_bucket_ptr) ( (cache_bucket_ptr)->tag_type & CACHE_TAG_MASK )
#define NEXT_BUCKET(cache_bucket_ptr) ( (cache_bucket_ptr)->next & ~CACHE_TAG_MASK )
#define BUCKET_DATA(cache_bucket_ptr) ( (void *) (cache_bucket_ptr)->data )

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


void configure_type(size_t size, unsigned preallocate);
    
void* stacked_malloc(size_t size);
void* kmalloc(size_t size);

#endif

