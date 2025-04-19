#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
 
#include "hashmap.h"
#include "cache.h"

int main() {
    srand(time(NULL));
    struct hashmap *hashmap = hashmap_new(sizeof(struct item), 0, 0, 0, hash, cmp, free_item, NULL);
    
    for (int i = 0; i < 100; i++) {
        struct item *item = random_item();
        hashmap_set(hashmap, (void *)item);
        free(item);
        /*
            Calling free() after inserting into the hashmap.
            If this is not called, it would result in a memory leak
        */
    }
    struct item* designed = item_from_kv("abc", "def");
    hashmap_set(hashmap, (void *)designed);
    free(designed);
    struct item get;
    get.key = "abc";
    struct item *res = hashmap_get(hashmap, (void *)&get);
    printf("key: %s, val: %s\n", res->key, res->val);
 
    hashmap_scan(hashmap, print_data, NULL);
 
    hashmap_free(hashmap);
 
    return 0;
}
