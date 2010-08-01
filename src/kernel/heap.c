#include "../inc/heap.h"
#include "../inc/utils.h"

// Una lista con una estructura por cada tamaño de tipo,
// Permite recuperar espacio vacio
static type_cache_t cache_lists[CACHE_COUNT] = { {} };

static void add_bucket(type_cache_t* cache, cache_bucket_t* bucket);
static cache_bucket_t* pop_bucket(type_cache_t* cache);
static type_cache_t* get_cache(size_t size);
static void grow_cache(type_cache_t* cache);

// Trae un objecto del tamaño size, pasando por los caches de memoria
void* kmalloc(size_t size) {
    type_cache_t* cache = get_cache(size); 

    if (cache->buckets == NULL) grow_cache(cache);
    cache_bucket_t* bucket = pop_bucket(cache);

    bucket->type_tag = CACHE_TYPE_TAG(cache);
    return BUCKET_TO_DATA(bucket);
}

// Libera un objeto a los caches q fue creado por kmalloc
void kfree(void* data) {
    cache_bucket_t* bucket = DATA_TO_BUCKET(data);
    type_cache_t* cache = &cache_lists[bucket->type_tag];
    add_bucket(cache, bucket);
}

static void grow_cache(type_cache_t* cache) {
  cache_bucket_t *buckets = allocate_pages(cache->grow_rate);
  
  for (int i = 0; i < CHUNK_TOTAL_BUCKETS(cache); i++)
    add_bucket(cache, &buckets[i]);
}

//Retorna el cache mas apropiado para guardar un objeto del tamaño 'size'
static type_cache_t* get_cache(size_t size) {
    for (int i = 0; i < CACHE_COUNT && cache_lists[0].bucket_size > 0; i++) {
        if (BUCKET_DATA_SIZE(&cache_lists[i]) > size)
            return &cache_lists[i];
   }

    kpanic("Objeto demasiado grande para ser guardado en los caches");
    return NULL; //Codigo muerto
}

// Saca un bucket del cache si este esta presente, si no retorna NULL
static cache_bucket_t* pop_bucket(type_cache_t* cache) {
    cache_bucket_t* result = cache->buckets;

    if (result) cache->buckets = result->next;
    return result;
}

// Agregar bucket a cache, funciona aun con la lista de buckets vacia 
static void add_bucket(type_cache_t* cache, cache_bucket_t* bucket) {
    memset(bucket, 0, cache->bucket_size);
    bucket->next = cache->buckets; 
    cache->buckets = bucket;
} 


// Crea un tipo de cache nuevo, en la lista cache list, de tamaño size,
// al crearlo inicializa al cache con preallocate entries vacias 
// Esta funcion debe ser llamada antes de la primer llamada a kmalloc, y NUNCA despues, durante la ejecucion del SO
void heap_configure_type(size_t size, long cache_pages) {
    int i;
    int bucket_size = ALIGN_TO_CACHE(size + 2, TRUE);
    
    type_cache_t tcache = { 
      .bucket_size = bucket_size > MIN_BUCKET_SIZE ? bucket_size : MIN_BUCKET_SIZE, 
      .grow_rate = cache_pages };
    type_cache_t tmp, *new_cache = &tcache;

    if (!tcache.grow_rate)
      tcache.grow_rate = (long) ALIGN_TO_PAGE(AVG_BUCKETS_PER_PAGE * bucket_size, TRUE)/PAGE_SIZE;
 
    if (cache_lists[CACHE_COUNT - 1].bucket_size != 0)
        kpanic("No puden crearse mas caches");

    //Ubica al cache de forma tal, q los de menor tamaño de bucket van primero
    for (i = 0; i < CACHE_COUNT && cache_lists[i].bucket_size != 0; i++) {
        if (tcache.bucket_size < cache_lists[i].bucket_size)
           return; // Usar el mismo cache para objetos parecidos 

        if (tcache.bucket_size < cache_lists[i].bucket_size) {
            tmp = cache_lists[i];
            cache_lists[i] = tcache;
            tcache = tmp;
        } 
    }
    cache_lists[i] = tcache;

    grow_cache(new_cache);
}

