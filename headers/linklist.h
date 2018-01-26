#ifndef __LIST_H
#define __LIST_H

typedef void (*freeFunction)(void *);

typedef enum { FALSE, TRUE } bool;

typedef bool (*listIterator)(void *);


typedef struct linklist_node {
  void *data;
  struct linklist_node *next;
} linklist_node;

typedef struct {
  int node_count;
  int node_size;
  linklist_node *head;
  linklist_node *tail;
  freeFunction freeFn;
} list;


void list_new(list *list, int node_size, freeFunction freeFn);
void list_destroy(list *list);
 
//void list_prepend(list *list, void *element);
void list_append(list *list, void *element);
int list_size(list *list);
 
void list_for_each(list *list, listIterator iterator);
void list_head(list *list, void *element, bool removeFromList);
void list_tail(list *list, void *element);

#endif
