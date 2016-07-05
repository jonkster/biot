#include <stdio.h>
#include <stdlib.h>

void addToDataCache(char *srcAdd, char* data)
{
    printf("saving %s from %s\n", data, srcAdd);
}

 
typedef struct {
    int size;
    void **keys;
    void **values;
} hash_t;
 
hash_t *hash_new (int size) {
    hash_t *h = calloc(1, sizeof (hash_t));
    h->keys = calloc(size, sizeof (void *));
    h->values = calloc(size, sizeof (void *));
    h->size = size;
    return h;
}
 
int hash_index (hash_t *h, void *key) {
    int i = (int) key % h->size;
    while (h->keys[i] && h->keys[i] != key)
        i = (i + 1) % h->size;
    return i;
}
 
void hash_insert (hash_t *h, void *key, void *value) {
    int i = hash_index(h, key);
    h->keys[i] = key;
    h->values[i] = value;
}
 
void *hash_lookup (hash_t *h, void *key) {
    int i = hash_index(h, key);
    void *r = h->values[i];
    return r;
}
 
int testHash (void) {
    puts("making hash");
    hash_t *h = hash_new(15);
    puts("adding stuff");
    hash_insert(h, "hello", "world");
    hash_insert(h, "a", "b");
    puts("getting stuff");
    void *r =  hash_lookup(h, "hello");
    printf("hello: %p\n", r);
    printf("hello: %s\n", (char*) r);

    r =  hash_lookup(h, "a");
    printf("a: %p\n", r);
    printf("a: %s\n", (char*) r);

    r =  hash_lookup(h, "herp");
    printf("herp: %p\n", r);
    if (r != 0)
        printf("herp: %s\n", (char*) r);

    /*printf("hello => %s\n", (char*) hash_lookup(h, "hello"));
    printf("herp => %s\n", (char*) hash_lookup(h, "herp"));
    printf("a => %s\n", (char*) hash_lookup(h, "a"));*/
    return 0;
}
