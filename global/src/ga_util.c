#include "ga_util.h"

bool llist_empty(llist_t * head) {
  if(head ==  NULL)
    return true;
  else
    return false;
}

int llist_size(llist_t * head) {
  llist_t * current;
  int count;
  if(head == NULL)
    return 0;
  else {
    count = 1;
    current = head;
    while(current->next != NULL) {
      ++count;
      current = current->next;
    }
    return count;
  }
}

void llist_insert_front(llist_t** head, int i) {
  llist_t * new_node;
  new_node  = malloc(sizeof(llist_t));
  new_node->val = i;
  new_node->next = *head;
  *head = new_node;
}

void llist_insert_back(llist_t** head, int i) {
  llist_t * current;
  current = *head;

  if(current == NULL) {
    current = malloc(sizeof(llist_t));
    current->val = i;
    current->next = NULL;
    (*head) = current;
  }
  else {
    while(current->next != NULL) {
      current = current->next;
    }
    current->next = malloc(sizeof(llist_t));
    current->next->val = i;
    current->next->next = NULL;
  }
}

void llist_insert(llist_t** head, int i) {
  llist_insert_back(head, i);
}

bool llist_remove_val(llist_t** head, int i) {
  llist_t * current = *head;
  llist_t * temp_node = NULL;

  int count = 0, break_count = -1, tmp;

  if(llist_empty(current)) {
    printf("List Empty!\n");
    return false;
  }
  else {
    while(current != NULL) {
      if(current->val == i) {
        break_count = count;
        break;
      }
      current = current->next;
      ++count;
    }
  }

  current = *head;

  if(break_count > (-1)) {
    /* printf("Entry Found!\n"); */
    if(break_count == 0) {
      temp_node = current;
      current = temp_node->next;
      free(temp_node);
      *head = current;
      return true;
    }
    else {
      for(tmp = 0; tmp < break_count - 1; ++tmp) {
        current = current->next;
      }
      temp_node = current->next;
      current->next  = temp_node->next;
      /* printf("Entry Found2 !\n"); */
      free(temp_node);
      return true;
    }
  }
  else {
    /* printf("Entry Not Found!\n"); */
    return false;
  }
}

bool llist_value_exists(llist_t* head, int i) {
  llist_t* current = head;
  while(current != NULL) {
    if(current->val == i)
      return true;
    current =  current->next;
  }
  return false;
}

void llist_print(llist_t* node) {
  if(node != NULL) {
    while(node != NULL) {
      printf("Current Value: %d\n",node->val);
      node = node->next;
    }
  }
  else {
    printf("Current List Empty\n");
  }
}

// #ifdef USE_DEVICE_MEM
//#ifdef CUDA_DEVICE

// #endif /* CUDA_DEVICE */
// #endif /* USE_DEVICE_MEM */
