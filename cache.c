#include "cache.h"

uint64_t hash(const void *item_ptr, uint64_t seed0, uint64_t seed1) {
    struct item *item = (struct item *)item_ptr;
    return hashmap_xxhash3((void *)item->key, strlen(item->key), seed0, seed1);
}

int cmp(const void *a, const void *b, void *udata) {
    const struct item* pa = a;
    const struct item* pb = b;
    return strcmp(pa->key, pb->key);
}

void free_item(void *item_ptr) {
    struct item *item = (struct item *)item_ptr;
    free(item->key); item->key = NULL;
    free(item->val); item->val = NULL;
}

struct item* item_from_kv(char *key, char *val) {
    struct item* item = (struct item *)malloc(sizeof(struct item));
    if (item == NULL) {
        fprintf(stderr, "Failed to malloc when creating item struct\n");
        return NULL;
    }
    
    item->key = malloc(sizeof(char) * (strlen(key) + 1));
    if (item->key == NULL) {
        fprintf(stderr, "Failed to malloc when creating item key\n");
        free(item);
        return NULL;
    }
    
    item->val = malloc(sizeof(char) * (strlen(val) + 1));
    if (item->val == NULL) {
        fprintf(stderr, "Failed to malloc when creating item value\n");
        free(item->key);
        free(item);
        return NULL;
    }
    
    strcpy(item->key, key);
    strcpy(item->val, val);
    return item;
} 

struct item *random_item() {
    struct item *item = (struct item *)malloc(sizeof(struct item));
    
    item->key = malloc(sizeof(char) * 11); 
    for (int i = 0; i < 10; i++) {
        
        int r = rand() % 62; 
        if (r < 10) item->key[i] = '0' + r; 
        else if (r < 36) item->key[i] = 'a' + (r - 10); 
        else item->key[i] = 'A' + (r - 36); 
    }
    item->key[10] = '\0'; 
    
    
    item->val = malloc(sizeof(char) * 6); 
    for (int i = 0; i < 5; i++) {
        int r = rand() % 62; 
        if (r < 10) item->val[i] = '0' + r; 
        else if (r < 36) item->val[i] = 'a' + (r - 10); 
        else item->val[i] = 'A' + (r - 36); 
    }
    item->val[5] = '\0'; 
    
    return item;
}
 
bool print_data(const void *item_ptr, __attribute__((unused)) void *udata) {
    struct item *item = (struct item *)item_ptr;
    printf("key: %s, val: %s\n", item->key, item->val);
    return true;
}