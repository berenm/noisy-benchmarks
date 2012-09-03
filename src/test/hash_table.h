#ifndef BNPPP_MAP_H_
#define BNPPP_MAP_H_

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#define HASH_TABLE_DEFAULT_SIZE 1048573

typedef struct _HashLink HashLink;
struct _HashLink {
  HashLink* hash_next;
  HashLink* list_next;
  HashLink* list_prev;

  uint64_t key;
  void*    value;
};

typedef struct _HashTable HashTable;

typedef void (HashTableDestroyValue)(void* value);

HashTable* HashTableCreate();
HashTable* HashTableCreateSized(size_t table_size);
void       HashTableDestroy(HashTable* table, HashTableDestroyValue* destroy_function);

void        HashTableInsert(HashTable* table, uint64_t const key, HashLink* link);
void*       HashTableRemove(HashTable* table, uint64_t const key);
void const* HashTableGet(HashTable const* table, uint64_t const key);

typedef void (HashTableForeachFun)(uint64_t key, void* value, void* argument);
void HashTableForEach(HashTable* table, HashTableForeachFun* function, void* argument);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BNPPP_MAP_H_ */
