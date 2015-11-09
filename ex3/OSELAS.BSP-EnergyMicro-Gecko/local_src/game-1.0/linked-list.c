#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>

struct coordinate
{
    int x;
    int y;
    struct coordinate *next;
};

struct coordinate *head = NULL;
struct coordinate *curr = NULL;

struct coordinate* create_list(int x, int y)
{
    struct coordinate *ptr = (struct coordinate*)malloc(sizeof(struct coordinate));
    if(NULL == ptr)
    {
        printf("\n Node creation failed \n");
        return NULL;
    }
    ptr->x = x;
    ptr->y = y;
    ptr->next = NULL;

    head = curr = ptr;
    return ptr;
}

struct coordinate* add_to_list(int x, int y, bool add_to_end)
{
    if(NULL == head)
    {
        return (create_list(x, y));
    }

    struct coordinate *ptr = (struct coordinate*)malloc(sizeof(struct coordinate));
    if(NULL == ptr)
    {
        printf("\n Node creation failed \n");
        return NULL;
    }
    ptr->x = x;
    ptr->y = y;
    ptr->next = NULL;

    if(add_to_end)
    {
        curr->next = ptr;
        curr = ptr;
    }
    else
    {
        ptr->next = head;
        head = ptr;
    }
    return ptr;
}

struct coordinate* search_in_list(int x, int y, struct coordinate **prev)
{
    struct coordinate *ptr = head;
    struct coordinate *tmp = NULL;
    bool found = false;

    while(ptr != NULL)
    {
        if(ptr->x == x && ptr->y == y)
        {
            found = true;
            break;
        }
        else
        {
            tmp = ptr;
            ptr = ptr->next;
        }
    }

    if(true == found)
    {
        if(prev)
            *prev = tmp;
        return ptr;
    }
    
    return NULL;
}

int delete_last()
{
    struct coordinate *last = NULL;
    struct coordinate *ptr = head;

    while(ptr != NULL)
    {
        if(ptr->next->next == NULL)
        {
            last = ptr->next;
            free(last);
            ptr->next = NULL;
            break;
        } else {
            ptr = ptr->next;
        }
    }
    return 0;
}


void print_list(void)
{
    struct coordinate *ptr = head;

    printf("\n -------Printing list Start------- \n");
    while(ptr != NULL)
    {
        printf("\n [x: %d, y: %d] \n",ptr->x, ptr->y);
        ptr = ptr->next;
    }
    printf("\n -------Printing list End------- \n");

    return;
}

int main(void)
{
    struct coordinate *ptr = NULL;

    for(int i = 1; i<10; i++)
        add_to_list(i,i,false);

    print_list();
    delete_last();
    delete_last();
    delete_last();
    delete_last();
    print_list();

    return 0;
}