#include "../inc/heap.h"
#include "../inc/utils.h"

// Una lista con una estructura por cada 
static type_cache_t cache_lists[CACHES_NUM] = { {} };

void* kmalloc(size_t size) {
    return stacked_malloc(size);
}

// Hace una allocacion naive, simplemente devolviendo un chunk de memoria contiguo de
// size_t bytes al final del la memoria utilizada por el kernel
// Siempre retorna una posicion alineada a CACHE_LINE
void* stacked_malloc(size_t size) {
    void *new = used_mem_limit;
    size = ALIGN_TO_CACHE(size, TRUE);

    used_mem_limit += size;   
    if (used_mem_limit > kernel_va_limit)
        ksbrk(size);
    
    return (void*) new;
}

void* add_bucket(type_cache_t* cache) {
    cache_bucket_t** where = NULL;
    if (cache->buckets == NULL)
        where = &cache->buckets;
    else {
        while ( (*where)->next )
            where = &(*where)->next;
    }
        
    *where = (cache_bucket_t *) stacked_malloc(cache->bucket_size);
    return *where;
} 


// Crea un tipo de cache nuevo, en la lista cache list, de tamaño size,
// all crearlo inicializa al cache con preallocate entries vacias, 
void configure_type(size_t size, unsigned preallocate) {
    int i, 
        bucket_size = ALIGN_TO_CACHE(size + 2, TRUE);
     
    type_cache_t tmp;
    type_cache_t tcache = { .bucket_size = bucket_size > MIN_BUCKET_SIZE ? bucket_size : MIN_BUCKET_SIZE }; 

    for (i = 0; i < preallocate; i++)
        add_bucket(&tcache);

    if (cache_lists[CACHES_NUM - 1].bucket_size != 0)
        kpanic("No puden crearse mas caches");

    for (i = 0; i < CACHES_NUM && cache_lists[i].bucket_size != 0; i++) {
        if (tcache.bucket_size < cache_lists[i].bucket_size) {
            tmp = cache_lists[i];
            cache_lists[i] = tcache;
            tcache = tmp;
        } 
    }    
    cache_lists[i] = tcache;
}

