#ifndef CACHE_H
#define CACHE_H

#include "hashmap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct item {
  char *key;
  char *val;
};

uint64_t hash(const void *item_ptr, uint64_t seed0, uint64_t seed1);
int cmp(const void *a, const void *b, void *udata);
void free_item(void *item_ptr);
struct item *item_from_kv(char *key, char *val);
struct item *random_item();
bool print_data(const void *item_ptr, __attribute__((unused)) void *udata);

#endif