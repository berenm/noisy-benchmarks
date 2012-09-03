#include "hash_table.h"

struct _HashTable {
  HashLink** links;
  HashLink   list_first;
  size_t     size;
};

HashTable* HashTableCreate() {
  return HashTableCreateSized(HASH_TABLE_DEFAULT_SIZE);
}

HashTable* HashTableCreateSized(size_t table_size) {
  HashTable* table = malloc(sizeof(HashTable));

  table->links = calloc(table_size, sizeof(HashLink*));
  table->size  = table_size;
  return table;
}

void HashTableDestroy(HashTable* table, HashTableDestroyValue* destroy_function) {
  if (destroy_function != NULL) {
    for (size_t i = 0; i < table->size; ++i) {
      HashLink* head = table->links[i];
      while (head != NULL) {
        destroy_function(head->value);
        head = head->hash_next;
      }
    }
  }

  free(table->links);
  free(table);
}

void HashTableInsert(HashTable* table, uint64_t key, HashLink* value) {
  HashLink** head = table->links + (key % table->size);

  value->key       = key;
  value->hash_next = *head;
  *head            = value;

  HashLink* first = &(table->list_first);
  value->list_next = first->list_next;
  value->list_prev = first;
  first->list_next = value;
}

void* HashTableRemove(HashTable* table, uint64_t const key) {
  HashLink** head = table->links + (key % table->size);

  while (*head != NULL) {
    if ((*head)->key == key) {
      HashLink* link = *head;
      *head = (*head)->hash_next;

      if (link->list_next != NULL) {
        link->list_next->list_prev = link->list_prev;
      }

      if (link->list_prev != NULL) {
        link->list_prev->list_next = link->list_next;
      }

      return link->value;
    }

    head = &(*head)->hash_next;
  }

  return NULL;
}

void const* HashTableGet(HashTable const* table, uint64_t const key) {
  HashLink* head = table->links[key % table->size];

  while (head != NULL) {
    if (head->key == key) {
      return head->value;
    }

    head = head->hash_next;
  }

  return NULL;
}

void HashTableForEach(HashTable* table, HashTableForeachFun function, void* argument) {
  HashLink* link = table->list_first.list_next;

  while (link != NULL) {
    function(link->key, link->value, argument);
    link = link->list_next;
  }
}
