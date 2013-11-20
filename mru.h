/* See LICENSE file for copyright and license details. */
#ifndef MRU_H
#define MRU_H

/********/
/* LIST */
/********/

/**
 * Linked list entry
 */
typedef struct ListEntry {
  struct ListEntry* next;       /* Next entry in chain */
  struct ListEntry* previous;   /* Previous entry in chain */
  char* val;                    /* Value */
} ListEntry;

/**
 * Create empty list.
 */
ListEntry* list_create();

/**
 * Destroy list and release all memory.
 */
void list_destroy(ListEntry* root);

/**
 * Add entry to double linked list.
 */
void list_add(ListEntry* root, char* val);

/**
 * Insert entry in a double linked list.  The entry will be inserted after e.  The new entry is
 * returned.
 */
ListEntry* list_insert(ListEntry* e, char* val);

/**
 * Remove entry from a double linked list.  Note that "e"'s memory will be freed.
 */
void list_remove(ListEntry* root);

/*************/
/* HASHTABLE */
/*************/

/**
 * Hashtable entry
 */
typedef struct HashEntry {
  struct HashEntry* next;       /* Next entry in chain */
  char* key;                    /* key */
  ListEntry* val;               /* value */
} HashEntry;

/**
 * Create hashtable and add all elements from the linked list.
 */
HashEntry** hash_create(unsigned size, ListEntry* head);

/**
 * Destroy hashtable.
 */
void hash_destroy(HashEntry** hashtable, unsigned size);

/**
 * Get entry.
 */
HashEntry* hash_get(HashEntry** hashtable, unsigned size, char* key);

/**
 * Add entry to hashmap.
 */
HashEntry* hash_put(HashEntry** hashtable, unsigned size, char* key, ListEntry* val);


#endif
