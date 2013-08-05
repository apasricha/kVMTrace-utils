/* VMT
This utility extracts memory adresses from its input and adds them to a FIFO queue if they are not alrady there. A hashtable is used to keep down the cost of looking up addresses.
*/

#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>

//Linked list for hash table (separate chaining, best?)
typedef struct _linked_list {

  uint64_t value;
  struct _linked_list *next;

}linked_list;

typedef struct _hash_table {

  int size;
  linked_list **table;

}hash_table;

typedef struct _queue {

  int first;
  int last;
  int count;
  int size;
  uint64_t *storage;
  hash_table *htable;

}queue;

hash_table *make_hash_table(int size)
{

  hash_table *new_table;
  int i;

  if (size < 1) return NULL; //invalid size

  //Attempt to allocate memory for table struc
  if ((new_table = malloc(sizeof(hash_table))) == NULL) {
    printf("Warning: htable mem allocation failed.");
    return NULL;
  }

  //Attempt to allocated memory for actual table
  if((new_table->table = malloc(sizeof(linked_list) * size)) == NULL) {
    printf("Warning: list table mem allocation failed.");
    return NULL;
  }

  //Initialize elements
  for (i = 0; i < size; i++) new_table->table[i] = NULL;

  new_table->size = size;

  return new_table;

}

int hash (hash_table *htable, uint64_t key)
{
  key >>= 3;
  return (key ^ (key>>10) ^ (key>>20)) & 0x3FF;
}

linked_list *lookup_addr(hash_table *htable, uint64_t addr)
{

  linked_list *list;
  unsigned int hashval = hash(htable, addr);

  for(list = htable->table[hashval]; list != NULL; list = list->next) {
    if (list->value == addr) return list;
  }

  printf("Address not in table");
  return NULL;

}

int remove_from_table(hash_table *htable, uint64_t addr)
{
  linked_list *temp;
  linked_list *temp_next;

  if((temp = lookup_addr(htable,addr)) == NULL) {
    printf("Warning: addr %" PRIx64 " not found, could not delete", addr);
  }

  //Bad practice, but this is a closed system so no possibility of external pointers
  temp_next = temp->next;
  temp->value = temp->next->value;
  temp->next = temp_next->next;
  free(temp_next);

  return 0;

}

int insert_to_table(hash_table *htable, uint64_t addr)
{

  linked_list *new_list;
  linked_list *current_list;
  unsigned int hashval = hash(htable, addr);

  //Attempt to alocate memory for list
  if ((new_list = malloc(sizeof(linked_list))) == NULL) {
    printf("Warning: list mem allocation failed.");
    return 1;
  }

  //Check: Already exists
  current_list = lookup_addr(htable, addr);
  if (current_list != NULL) return 2;

  new_list->value = addr;
  new_list->next = htable->table[hashval];
  htable->table[hashval] = new_list;

  return 0;

}

void free_table(hash_table *htable)
{

  int i;
  linked_list *list, *temp;

  if (htable == NULL) return;

  for (i = 0; i < htable->size; i++) {
    list = htable->table[i];
    while(list!=NULL) {
      temp = list;
      list = list->next;
      free(temp->value);
      free(temp);
    }
  }

  free(htable->table);
  free(htable);
}

queue *make_queue(int size, hash_table *htable)
{
  queue *new_queue;
  int i;

  if (size < 1) return NULL; //invalid size

 //Attempt to allocate memory for queue struc
  if ((new_queue = malloc(sizeof(queue))) == NULL) {
    printf("Warning: queue mem allocation failed.");
    return NULL;
  }

  new_queue->size = size;
  new_queue->first = 0;
  new_queue->last = size-1;
  new_queue->count = 0;
  new_queue->htable = htable;

  //Attempt to allocate memory for storage
  if((new_queue->storage = malloc(sizeof(uint64_t)*size)) == NULL) {
    printf("Warning: storage mem allocation failed.");
    return NULL;
  }

  //Initialize elements
  for (i = 0; i < size; i++) new_queue->storage[i] = NULL;

  return new_queue;

}

uint64_t remove_from_queue (queue *q)
{
  uint64_t rem;

  if (q->count <= q->size) {
    printf("Warning: Queue is not full.");
  } else {
    rem = q->storage[q->first];
    q->first = (q->first+1)%q->size;
    q->count--;
  }

  remove_from_table(q->htable,rem); 

  return rem;

}

int insert_to_queue (queue *q, uint64_t addr)
{
  if(q->count >= q->size) {
    printf("Warning: queue is full, %" PRIx64 " removed",remove_from_queue(q));
  }

  q->last = (q->last+1) % q->size;
  q->storage[q->last] = addr;
  q->count++;

  insert_to_table(q->htable, addr);

  return 0;

}

void free_queue(queue *q)
{

  free(q->storage);
  free(q);

}

int main(int argc, char *argv[])
{
  uint64_t address;
  int FIFO_size = (int)argv[1];
  hash_table *lookup_table;
  queue *FIFO_queue;
  linked_list *list;

  if((lookup_table = make_hash_table(FIFO_size)) == NULL) {
    return 1; 
  }

  if((FIFO_queue = make_queue(FIFO_size, lookup_table)) == NULL) {
    return 2;
  }

  while((scanf("%"PRIx64"",&address)) == 1) {
    if ((list = lookup_addr(lookup_table, address)) != NULL) {
      printf("Address already in queue.");
    } else {
      insert_to_queue(FIFO_queue, address);
      printf("Address %" PRIx64 " was added to queue", address);
    }

  }

  return 0;
}
