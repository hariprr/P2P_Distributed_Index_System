#include <stdlib.h>
#include <string.h>
#include <assert.h>
 
#include "../headers/linklist.h"

 
void list_new(list *list, int node_size, freeFunction freeFn)
{
  list->node_count = 0;
  list->node_size = node_size;
  list->head = list->tail = NULL;
  list->freeFn = freeFn;
}
 
void list_destroy(list *list)
{
  linklist_node *current;
  while(list->head != NULL) {
    current = list->head;
    list->head = current->next;

    if(list->freeFn) {
      list->freeFn(current->data);
    }

    free(current->data);
    free(current);
  }
}
/* 
void list_prepend(list *list, void *element)
{
  linklist_node *node = malloc(sizeof(linklist_node));
  node->data = malloc(list->node_size);
  memcpy(node->data, element, list->node_size);

  node->next = list->head;
  list->head = node;

  // first node?
  if(!list->tail) {
    list->tail = list->head;
  }

  list->node_count++;
}
*/

void list_append(list *list, void *element)
{
  linklist_node *node = malloc(sizeof(linklist_node));
  node->data = malloc(list->node_size);
  node->next = NULL;

  memcpy(node->data, element, list->node_size);

  if(list->node_count == 0) {
    list->head = list->tail = node;
  } else {
    list->tail->next = node;
    list->tail = node;
  }

  list->node_count++;
}
/* 
void list_for_each_search(list *list, listIterator iterator)
{
  assert(iterator != NULL);
 
  linklist_node *node = list->head;
  bool result = TRUE;
  while(node != NULL && result) {
    result = iterator(node->data);
    node = node->next;
  }
}
*/

void list_for_each(list *list, listIterator iterator)
{
  assert(iterator != NULL);
 
  linklist_node *node = list->head;
  bool result = TRUE;
  while(node != NULL && result) {
    result = iterator(node->data);
    node = node->next;
  }
}
 
void list_head(list *list, void *element, bool removeFromList)
{
  assert(list->head != NULL);
 
  linklist_node *node = list->head;
  memcpy(element, node->data, list->node_size);
 
  if(removeFromList) {
    list->head = node->next;
    list->node_count--;
 
    free(node->data);
    free(node);
  }
}
 
void list_tail(list *list, void *element)
{
  assert(list->tail != NULL);
  linklist_node *node = list->tail;
  memcpy(element, node->data, list->node_size);
}
 
int list_size(list *list)
{
  return list->node_count;
}
