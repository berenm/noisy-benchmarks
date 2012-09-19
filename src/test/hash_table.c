#include "test/hash_table.h"

HashTable* HashTableCreate() {
  return HashTableCreateSized(HASH_TABLE_DEFAULT_SIZE);
}

HashTable* HashTableCreateSized(size_t table_size) {
  HashTable* table = (HashTable*) malloc(sizeof(HashTable));

  table->links = (HashLink**) calloc(table_size, sizeof(HashLink*));
  table->size  = table_size;

  return table;
}

void HashTableDestroy(HashTable* table, HashTableDestroyValue* destroy_function) {
  if (destroy_function != NULL) {
    for (size_t i = 0; i < table->size; ++i) {
      HashLink* hash_link = table->links[i];
      while (hash_link != NULL) {
        void* value = hash_link->value;
        hash_link = hash_link->hash_next;
        destroy_function(value);
      }
    }
  }

  free(table->links);
  free(table);
}

void HashTableInsert(HashTable* table, HashLink* link) {
  HashLink** head = table->links + (HashTableFastHash(&link->key) % table->size);

  link->hash_next = *head;
  *head           = link;
}

void* HashTableRemove(HashTable* table, HashLink* link) {
  HashLink** head = table->links + (HashTableFastHash(&link->key) % table->size);

  while (*head != NULL) {
    if (*head == link) {
      *head           = link->hash_next;
      link->hash_next = NULL;
      return link->value;
    }

    head = &(*head)->hash_next;
  }

  return NULL;
}

void HashTableForEach(HashTable* table, HashTableForeachFun function, void* argument) {
  for (size_t i = 0; i < table->size; ++i) {
    HashLink* hash_link = table->links[i];
    while (hash_link != NULL) {
      function(&hash_link->key, hash_link->value, argument);
      hash_link = hash_link->hash_next;
    }
  }
}
