#include<stdio.h>  
#include<stdlib.h>  
#include<malloc.h>  

#include "typedefs.h"
#include "dfs_node.h"
#include "sparse_parent_bitwise.h"

typedef struct REPAIR_LIST  
{  
    varset data;  
    struct REPAIR_LIST *prev;  
    struct REPAIR_LIST *next;  
}repair_list;

/**
 * Create a doubly linked list with n elements and enter the elements
 */
repair_list *init_list(varset allvariables)
{
    repair_list *head,*q;
    head = (repair_list *)malloc(sizeof(repair_list));
    q = (repair_list *)malloc(sizeof(repair_list));
    q->data = allvariables;
    head->prev = q;
    head->next = q;
    q->prev = head;
    q->next = head;
    return head;
}

/**
 * Traverse and output these elements
 */
void print_list(repair_list *head)
{
    repair_list *p;
    p = head;
    p = p->next;
    while(p != head)
    {
        varset variables = p->data;
        printf("%s \n", varsetToString(variables).c_str());
        p = p->next;
    }
    printf("\n");
}

/**
 * Get the number of elements in the doubly linked list
 */
int length_list(repair_list *head)
{
    repair_list *p;
    p = head;
    p = p->next;
    int count = 0;
    while(p != head)
    {
        count++;
        p = p->next;
    }
    return count;
}

/**
 * Determine whether the number of elements in the list is greater than the number of variables
 */
bool isLess(repair_list *head, int variableCount)
{
    repair_list *p;
    p = head;
    p = p->next;
    int count = 0, sign = 0;
    while(p != head)
    {
        count++;
        p = p->next;
        if(count >= variableCount*2) {sign = 1; break;}
    }
    if(sign == 1) return true;
        else return false;
}

/**
 * Insert data after the specified element
 */
void insert_back(repair_list* d, varset data)
{
    repair_list *q;
    q = (repair_list *)malloc(sizeof(repair_list));
    q->data = data;
    q->next = d->next;
    d->next->prev = q;
    q->prev = d;
    d->next = q;
}

/**
 * Insert data before the specified element data
 */
void insert_front(repair_list* d, varset data)
{
    repair_list *q;
    q = (repair_list *)malloc(sizeof(repair_list));
    q->data = data;
    q->prev = d->prev;
    d->prev->next = q;
    q->next = d;
    d->prev = q;
}

/**
 * Insert data data before the i-th element
 */
void insert_before_i(repair_list *head, int i, varset data)
{
    repair_list *p = head, *q;
    p = p->next;
    i--;
    while(i--)
        p = p->next;
    q = (repair_list *)malloc(sizeof(repair_list));
    q->data = data;
    (p->prev)->next = q;
    q->prev = p->prev;
    q->next = p;
    p->prev = q;
}

/**
 * Delete an element at the current position
 */
void delete_now(repair_list* d)
{
    repair_list* p = d;
    (p->prev)->next = p->next;
    (p->next)->prev = p->prev;
    free(p);
}

/**
 * Delete the element at the i-th position
 */
void delete_i(repair_list *head, int i)
{
    repair_list *p = head;
    p = p->next;
    i--;
    while(i--)
        p = p->next;
    (p->prev)->next = p->next;
    (p->next)->prev = p->prev;
    free(p);
}

/**
 * Delete element with value x
 */
void delete_value_x(repair_list *head, varset variables)
{
    repair_list *p = head, *q;
    p = p->next;
    while(p != head)
        if(p->data == variables)
        {
            q = p->next;
            (p->prev)->next = p->next;
            (p->next)->prev = p->prev;
            free(p);
            p = q;
        }
        else p = p->next;
}