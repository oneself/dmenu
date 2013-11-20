/* See LICENSE file for copyright and license details. */
#include <dirent.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "mru.h"

/********/
/* LIST */
/********/

/**
 * Free memory for one list entry.  Null entries will be silently ignored.
 */
void _list_entry_destroy(ListEntry* e) {
  if (e != NULL) {
    free(e->val);               /* Remember to free the value */
    free(e);
  }
}

ListEntry *list_create() {
  ListEntry *e = (ListEntry*) malloc(sizeof(ListEntry*)); /* Allocate new entry */
  e->next = e;                  /* This is the root entry so next and previous are set to self */
  e->previous = e;
  e->val = NULL;                /* No value in the root entry, it will remain null */
  return e;
}

void list_add(ListEntry *root, char *val) {
  ListEntry *n = (ListEntry*) malloc(sizeof(ListEntry*)); /* Allocate new entry */
  n->val = strdup(val);         /* Copy and set the value */
  n->next     = root;           /* Insert before root which is the last item */
  n->previous = root->previous;
  root->previous->next = n;
  root->previous = n;
}

ListEntry *list_insert(ListEntry *e, char *val) {
  ListEntry *n = (ListEntry*) malloc(sizeof(ListEntry*)); /* Allocate new entry */
  n->val = strdup(val);         /* Copy and set the value */
  n->previous = e;
  n->next = e->next;
  e->next = n;
  return n;
}

void list_remove(ListEntry *e) {
  ListEntry *p = e->previous;   /* Get previous and next entries */
  ListEntry *n = e->next;
  p->next = e->next;
  n->previous = e->previous;
  _list_entry_destroy(e);       /* Remember to free memory */
}

void list_destroy(ListEntry *root) {
  ListEntry* p = NULL;          /* Save previous value for each iteration, since we can't free
                                   memory for the current entry before moving to the next */
  ListEntry *e = NULL;
  for(e = root->next; e != root; p = e, e = e->next) {
    _list_entry_destroy(p);
  }

  _list_entry_destroy(p);                 /* Remember to free the last element */
}

/*************/
/* HASHTABLE */
/*************/

/**
 * Calculate hash value for the given string.
 */
unsigned _hash(char *s, unsigned size) {
  unsigned hashval;
  for (hashval = 0; *s != '\0'; ++s)
    hashval = *s + 31 * hashval;
  return hashval % size;
}

HashEntry **hash_create(unsigned size, ListEntry *root) {
  HashEntry **hashtable = (HashEntry**) malloc(size * sizeof(HashEntry*));
  ListEntry *e;
  for(e = root->next; e != root; e = e->next) {
    hash_put(hashtable, size, e->val, e);
  }
  return hashtable;
}

void hash_destroy(HashEntry **hashtable, unsigned size) {
  int i;
  for (i = 0; i < size; ++i) {
    if (hashtable[i] != NULL) {
      free(hashtable[i]->key);
      free(hashtable[i]);
    }
  }
  free(hashtable);
}

HashEntry *hash_get(HashEntry **hashtable, unsigned size, char *key) {
  HashEntry *e = NULL;
  for (e = hashtable[_hash(key, size)]; e != NULL; e = e->next) {
    if (strcmp(key, e->key) == 0)
      return e;
  }
  return NULL;
}

HashEntry *hash_put(HashEntry **hashtable, unsigned size, char *key, ListEntry *val) {
  HashEntry *e = NULL;
  unsigned h;
  if (key == NULL) {
    return NULL;                /* Will not add empty keys */
  }
  e = hash_get(hashtable, size, key);
  if (e != NULL) {              /* Existing entry found.  We'll just update it */
    e->val = val;
  } else {                      /* Entry not found. Need a new one */
    e = (HashEntry*) malloc(sizeof(*e)); /* Allocate new Entry */
    e->key = strdup(key);                /* Update entry's key */
    h = _hash(key, size); /* Calculate the hash value */
    e->next = hashtable[h];     /* Push this entry to the top of the list */
    e->val = val;
    hashtable[h] = e;           /* Add new entry */
  }
  return e;
}
