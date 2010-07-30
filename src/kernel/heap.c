#include "../inc/heap.h"
#include "../inc/utils.h"

// Una lista con una estructura por cada tamaño de tipo,
// Permite recuperar espacio vacio
static type_cache_t cache_lists[CACHES_NUM] = { {} };

static void add_bucket(type_cache_t* cache, cache_bucket_t* bucket);
static cache_bucket_t* pop_bucket(type_cache_t* cache);
static type_cache_t* get_cache(size_t size);

// Trae un objecto del tamaño size, pasando por los caches de memoria
void* kmalloc(size_t size) {
    type_cache_t* cache = get_cache(size); 
    cache_bucket_t* bucket;

    if (cache->buckets)
        bucket = pop_bucket(cache);
    else 
        bucket = stacked_malloc(cache->bucket_size);

    bucket->type_tag = CACHE_TYPE_TAG(cache);

    return BUCKET_TO_DATA(bucket);
}

// Libera un objeto a los caches q fue creado por kmalloc
void kfree(void* data) {
    cache_bucket_t* bucket = DATA_TO_BUCKET(data);
    add_bucket(&cache_lists[bucket->type_tag], bucket);
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

//Retorna el cache mas apropiado para guardar un objeto del tamaño size
static type_cache_t* get_cache(size_t size) {
    for (int i = 0; i < CACHES_NUM && cache_lists[0].bucket_size > 0; i++) {
        if (BUCKET_DATA_SIZE(&cache_lists[i]) > size)
            return &cache_lists[i];
    }

    kpanic("Objeto demasiado grande para ser guardado en los caches");
    return NULL; //Codigo muerto
}

static cache_bucket_t* pop_bucket(type_cache_t* cache) {
    cache_bucket_t* result = cache->buckets;

    if (cache->buckets) cache->buckets = result->next;

    return result;
}

// 
static void add_bucket(type_cache_t* cache, cache_bucket_t* bucket) {
    if (bucket == NULL) 
        bucket = (cache_bucket_t *) stacked_malloc(cache->bucket_size);

    memset(bucket, 0, cache->bucket_size);
    bucket->next = cache->buckets; 
    cache->buckets = bucket;
} 


// Crea un tipo de cache nuevo, en la lista cache list, de tamaño size,
// al crearlo inicializa al cache con preallocate entries vacias 
// Esta funcion debe ser llamada antes de la primer llamada a kmalloc, y NUNCA despues, durante la ejecucion del SO
void configure_type(size_t size, unsigned preallocate) {
    int i;
    int bucket_size = ALIGN_TO_CACHE(size + 2, TRUE);
     
    type_cache_t tmp;
    type_cache_t tcache = { .bucket_size = bucket_size > MIN_BUCKET_SIZE ? bucket_size : MIN_BUCKET_SIZE }; 

    for (i = 0; i < preallocate; i++) add_bucket(&tcache, NULL);

    if (cache_lists[CACHES_NUM - 1].bucket_size != 0)
        kpanic("No puden crearse mas caches");

    //Ubica al cache de forma tal, q los de menor tamaño de bucket van primero
    for (i = 0; i < CACHES_NUM && cache_lists[i].bucket_size != 0; i++) {
        if (tcache.bucket_size < cache_lists[i].bucket_size) {
            tmp = cache_lists[i];
            cache_lists[i] = tcache;
            tcache = tmp;
        } 
    }    
    cache_lists[i] = tcache;
}

