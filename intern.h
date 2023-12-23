#ifndef intern_h
#define intern_h
#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<string.h>
#include<assert.h>
#define MAX_CAPACITY 128
#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL
// reference: https://benhoyt.com/writings/hash-table-in-c/
typedef struct {
    char* key;
} ht_entry;

typedef struct {
    ht_entry *entries;
    size_t capacity;
    size_t length;
} ht;

uint64_t hash_key(char* key);
ht* ht_create(void);
char* ht_set_entry(ht_entry* entries, size_t capacity, char* key, size_t* plength);
void ht_dump(ht *t);
char* intern(ht *table, char* key);

ht* ht_create(void) {
    ht* table = (ht *) malloc(sizeof(ht));
    table->length = 0;
    table->capacity = MAX_CAPACITY;
    table->entries = (ht_entry*) calloc(table->capacity, sizeof(ht_entry));
    return table;
}

// Return 64-bit FNV-1a hash for key (NUL-terminated). See description:
// https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function
uint64_t hash_key(char* key) {
    uint64_t hash = FNV_OFFSET;
    for (const char* p = key; *p; p++) {
        hash ^= (uint64_t)(unsigned char)(*p);
        hash *= FNV_PRIME;
    }
    return hash;
}

char* intern(ht *table, char* key) {
    assert(table->length < table->capacity);
    return ht_set_entry(table->entries, table->capacity, key,
                        &table->length);
}

char* ht_set_entry(ht_entry* entries, size_t capacity,
                   char* key, size_t* plength) {
    assert(plength != NULL);
    uint64_t hash = hash_key(key);
    size_t index = (size_t)(hash & (uint64_t)(capacity - 1));
    
    // Loop till we find an empty entry.
    while (entries[index].key != NULL) {
        if (strcmp(key, entries[index].key) == 0) {
            return entries[index].key;
        }
        // Key wasn't in this slot, move to next (linear probing).
        index++;
        if (index >= capacity) {
            // At end of entries array, wrap around.
            index = 0;
        }
    }
    // Didn't find key, allocate+copy if needed, then insert it.
    key = strdup(key);
    entries[index].key = key;
    (*plength)++;
    return key;
}

void ht_dump(ht *t){
    printf("(ht){\n");
    for(size_t i = 0; i < t->capacity; ++i){
        if(t->entries[i].key != NULL){
            printf("\tkey=%s,\n",t->entries[i].key);
        }
    }
    printf("}");
}

#endif
