#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#include "hashmap.h"

 
struct item {
    char *str;
    int x;
};
 
uint64_t hash(const void *item_ptr, uint64_t seed0, uint64_t seed1) {
    struct item *item = (struct item *)item_ptr;
    return hashmap_xxhash3((void *)item->str, strlen(item->str), seed0, seed1);
}
 
int cmp(const void *aptr, const void *bptr, __attribute__((unused)) void *udata) {
    struct item *a = (struct item *)aptr;
    struct item *b = (struct item *)bptr;
 
    if (a->x < b->x) return -1;
    if (a->x > b->x) return 1;
    return 0;
}
 
void free_item(void *item_ptr) {
    struct item *item = (struct item *)item_ptr;
    free(item->str); item->str = NULL;
    /**
     * free(item); item = NULL;
     * you cannot call this here, item points to some memory inside map->buckets.
     * map->buckets is freed by hashmap_free() later.
     */
}
 
struct item *create_item() {
    struct item *item = (struct item *)malloc(sizeof(struct item));
    item->str = malloc(sizeof(char) * 10);
    memcpy(item->str, "123456789", 10);
    item->x = rand();
    return item;
}
 
bool print_data(const void *item_ptr, __attribute__((unused)) void *udata) {
    struct item *item = (struct item *)item_ptr;
    printf("string: %s\n", item->str);
    return true;
}
 
int main() {
    struct hashmap *hashmap = hashmap_new(sizeof(struct item), 0, 0, 0, hash, cmp, free_item, NULL);
    
    struct item *item = create_item();
    hashmap_set(hashmap, (void *)item);
 
    /*
        Calling free() after inserting into the hashmap.
        If this is not called, it would result in a memory leak
    */
    free(item);
 
    hashmap_free(hashmap);
 
    return 0;
}
