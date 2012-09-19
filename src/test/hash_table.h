#ifndef HASH_TABLE_H_
#define HASH_TABLE_H_

#include <string.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

// nice primes for hash table size
// 100003 199999 400009 1000003
#define HASH_TABLE_DEFAULT_SIZE 1000003

typedef struct _HashKey HashKey;
struct _HashKey
{
    uint64_t data[2];
    uint8_t  data_length;
};

typedef struct _HashLink HashLink;
struct _HashLink
{
    HashLink* hash_next;
    HashKey   key;
    void*     value;
};

typedef struct _HashTable HashTable;
struct _HashTable
{
    size_t     size;
    HashLink** links;
};

typedef void (HashTableDestroyValue)(void* value);

HashTable* HashTableCreate();
HashTable* HashTableCreateSized(size_t table_size);
void       HashTableDestroy(HashTable* table, HashTableDestroyValue* destroy_function);

void  HashTableInsert(HashTable* table, HashLink* link);
void* HashTableRemove(HashTable* table, HashLink* link);

typedef void (HashTableForeachFun)(HashKey const* key, void* value, void* argument);
void HashTableForEach(HashTable* table, HashTableForeachFun* function, void* argument);

static inline uint32_t HashTableFastHash(HashKey const* key)
{
    if (key->data_length == 1)
        return key->data[0] & 0xFFFFFFFF;

    uint8_t const* data        = (uint8_t const*) key->data;
    uint32_t       data_length = key->data_length * sizeof(uint64_t);

    uint32_t hash = data_length;
    uint32_t tmp;

    if ((data_length <= 0) || (data == NULL))
        return 0;

    uint32_t const remaining = data_length & 3;
    data_length >>= 2;

    /* Main loop */
    for (; data_length > 0; --data_length)
    {
        hash += *((uint16_t const*) (data));
        tmp   = (*((uint16_t const*) (data + 2)) << 11) ^ hash;
        hash  = (hash << 16) ^ tmp;
        data += 2 * sizeof(uint16_t);
        hash += hash >> 11;
    }

    /* Handle end cases */
    switch (remaining)
    {
        case 3:
            hash += *((uint16_t const*) (data));
            hash ^= hash << 16;
            hash ^= ((int8_t) data[sizeof(uint16_t)]) << 18;
            hash += hash >> 11;
            break;

        case 2:
            hash += *((uint16_t const*) (data));
            hash ^= hash << 11;
            hash += hash >> 17;
            break;

        case 1:
            hash += (int8_t) *data;
            hash ^= hash << 10;
            hash += hash >> 1;
    }

    /* Force "avalanching" of final 127 bits */
    hash ^= hash << 3;
    hash += hash >> 5;
    hash ^= hash << 4;
    hash += hash >> 17;
    hash ^= hash << 25;
    hash += hash >> 6;

    return hash;
} /* HashTableFastHash */

static int HashKeyEq(HashKey const* key_a, HashKey const* key_b)
{
    if (key_a->data_length != key_b->data_length)
        return 0;

    switch (key_a->data_length)
    {
        case 2:
            if (key_a->data[1] != key_b->data[1])
                return 0;

        case 1:
            if (key_a->data[0] != key_b->data[0])
                return 0;
    }

    return 1;
}

static inline void* HashTableGet(HashTable const* table, HashKey const* key)
{
    HashLink const* head = table->links[HashTableFastHash(key) % table->size];

    while (head != NULL)
    {
        if (HashKeyEq(&head->key, key))
            return head->value;

        head = head->hash_next;
    }

    return NULL;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HASH_TABLE_H_ */
