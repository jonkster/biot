#ifndef _DATACACHE_H_
#define _DATACACHE_H_

typedef struct {
    uint16_t size;
    uint16_t currentSize;
    char **keys;
    char **values;
    char **keySet;
} hash_t;


#ifdef __cplusplus
extern "C" {
#endif

 

hash_t *newHash(uint32_t size);

bool deleteEntry(hash_t *h, char *key);
bool exists(hash_t *h, char *key);
char *getValue (hash_t *h, char *key);
void setValue (hash_t *h, char *key, char *value);
int testHash(void);

#ifdef __cplusplus
}
#endif
#endif
