#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <string.h>

#include "dataCache.h"

/*bool testInit(hash_t *h)
{
// problems with malloc in RIOT for 32 bit uints - this part of debugging -
// remove when sorted
    //printf("configured size:%d\n", h->size);
    for (uint16_t i = 0; i < h->size; i++)
    {
        if (h->slotUsed[i])
        {
            printf("expected slot %u to be false!\n", i);
            return false;
        }

        if (h->keys[i] != 0)
        {
            printf("expected keys %u to be 0 not: %p !\n", i, h->keys[i]);
            return false;
        }

        if (h->values[i] != 0)
        {
            printf("expected values %u to be 0 not: %p !\n", i, h->values[i]);
            return false;
        }

        if (h->keySet[i] != 0)
        {
            printf("expected keySet %u to be 0 not: %p !\n", i, h->keySet[i]);
            return false;
        }
    }
    return true;
}*/

hash_t *newHash (uint16_t size)
{
    hash_t *h = malloc(sizeof (hash_t));
    h->size = size;
    h->currentSize = 0;

    h->slotUsed = malloc(size * sizeof (bool));

    h->keys = calloc(size, sizeof (char*));

    h->values = calloc(size, sizeof (char*));

    h->keySet = calloc(size, sizeof (char*));

    for (uint16_t i = 0; i < size; i++)
        h->slotUsed[i] = false;

  /*  if (! testInit(h))
    {
        printf("Woah! error initialising!\n");
        exit(1);
    }*/


    return h;
}

/*
 * djb2 hash function by Dan Bernstein - http://www.cse.yorku.ca/~oz/hash.html
 */
uint16_t djb2Hash(char *key)
{
    uint16_t hash = 5381;
    int c;

    while ((c = *key++) != 0)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}
 
int16_t hashKey (hash_t *h, char *key) 
{
    uint16_t i = djb2Hash(key) % h->size;
    int16_t overflow = 1;
    while ((h->keys[i] != 0) && (strcmp(h->keys[i], key) != 0))
    {
        i = (i + 1) % h->size;
        if (overflow++ >= h->size)
        {
            // degenerate case - not a stored entry and hash table full
            return -1;
        }
    }
    return i;
}

void allKeys(hash_t *h)
{
    uint16_t kIdx = 0;
    h->keySet[kIdx] = 0;
    for (uint16_t i = 0; i < h->size; i++)
    {
        if (kIdx >= h->currentSize)
            break;
        if (h->keys[i] != 0)
        {
            h->keySet[kIdx] = h->keys[i];
            kIdx++;
        }
    }
}

bool exists(hash_t *h, char *key)
{
    int16_t i = hashKey(h, key);
    if (i < 0)
        return false;

    return (h->slotUsed[i]);
}



 
void setValue (hash_t *h, char *key, char *value) 
{
    bool alreadyGot = exists(h, key);
    bool stillRoom = h->currentSize < h->size;
    if (alreadyGot || stillRoom) 
    {
        int16_t i = hashKey(h, key);
        if (i >= 0)
        {
            if (alreadyGot)
            {
                free(h->keys[i]);
                h->keys[i] = 0;

                free(h->values[i]);
                h->values[i] = 0;

                h->slotUsed[i] = false;
            }
            else
            {
                h->currentSize++;
            }

            char *k = (char*) malloc(strlen(key) + 1);
            memcpy(k, key, strlen(key) + 1);
            h->keys[i] = k;

            char *v = (char*) malloc(strlen(value) + 1);
            memcpy(v, value, strlen(value) + 1);
            h->values[i] = v;

            h->slotUsed[i] = true;

            allKeys(h);
            return;
        }
    }
    puts("hash overflow!!");
    return;
}

bool deleteEntry(hash_t *h, char *key)
{
    int16_t i = hashKey(h, key);
    if (i >= 0)
    {
        if (exists(h, key))
        {
            free(h->keys[i]);
            h->keys[i] = 0;
            free(h->values[i]);
            h->values[i] = 0;
            h->currentSize--;

            h->slotUsed[i] = false;

            return true;
        }
    }
    return false;
}
 
char *getValue (hash_t *h, char *key) 
{
    int16_t i = hashKey(h, key);
    if (i >= 0)
    {
        if (h->slotUsed[i])
        {
            char *r = h->values[i];
            return r;
        }
        else
        {
            printf("cannot find %s???\n", key);
        }
    }
    return "";
}

void dumpHash(hash_t *h)
{
    uint16_t count = h->currentSize;
    printf("hash has %d values\n", count);
    for (uint8_t i = 0; i < count; i++)
    {
        char *key = h->keySet[i];
        printf("key:'%s' = ", key);
        printf("'%s'\n", getValue(h, key));
    }
}

int testHash (void) 
{
    puts("making hash");
    hash_t *h = newHash(2);

    puts("adding stuff");
    setValue(h, "hello", "world");
    puts("dump hash");
    dumpHash(h);
    setValue(h, "a", "b");
    puts("dump hash");
    dumpHash(h);

    puts("getting stuff");
    if (exists(h, "hello"))
        printf("hello = %s\n", getValue(h, "hello"));
    else
        puts("hello has no value");

    if (exists(h, "herps"))
        printf("herps = %s\n", getValue(h, "herps"));
    else
        puts("herps has no value");

    if (exists(h, "a"))
        printf("a = %s\n", getValue(h, "a"));
    else
        puts("a has no value");

    puts("overflowing stuff");
    setValue(h, "c", "d");
    if (exists(h, "c"))
        printf("c = %s\n", getValue(h, "c"));
    else
        puts("c has no value");
    puts("dump hash");
    dumpHash(h);

    puts("replace value");
    setValue(h, "hello", "Jupiter!");
    if (exists(h, "hello"))
        printf("hello = %s\n", getValue(h, "hello"));
    else
        puts("hello has no value");
    puts("dump hash");
    dumpHash(h);

    puts("delete value");
    bool existed = deleteEntry(h, "hello");
    if (existed)
    {
        puts("hello deleted");
    }
    else
    {
        puts("hello never existed");
    }

    if (exists(h, "hello"))
        printf("hello = %s\n", getValue(h, "hello"));
    else
        puts("hello has no value");
    puts("dump hash");
    dumpHash(h);


    puts("try to delete non existant value");
    existed = deleteEntry(h, "fred");
    if (existed)
    {
        puts("fred deleted");
    }
    else
    {
        puts("fred never existed");
    }
    if (exists(h, "fred"))
        printf("fred = %s\n", getValue(h, "fred"));
    else
        puts("fred has no value");

    puts("dump hash");
    dumpHash(h);

    int s = 200;
    hash_t *h2 = newHash(s);
    for (int i = 0; i < s; i++)
    {
        char k[5];
        char v[20];
        sprintf(k, "k-%d", i);
        sprintf(v, "value = %d", i);
        setValue(h2, k, v);
    }
    puts("dump hash");
    dumpHash(h2);
    for (int i = 0; i < s; i++)
    {
        char k[5];
        char v[20];
        sprintf(k, "k-%d", i);
        sprintf(v, "v = %d", i);
        setValue(h2, k, v);
    }
    puts("dump hash");
    dumpHash(h2);


    return 0;
}

