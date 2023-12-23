#ifndef queue_h
#define queue_h
#include"./obj.h"
#include"./memory.h"
#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>

typedef struct T {char* data; int8_t token;} T;

typedef struct node {
    T data;
    struct node* next;
} node;

typedef struct queue{
    struct node* front;
    struct node* end;    
} queue;

node* create_node();
void enqueue(queue* q, T data);
T dequeue(queue* q);
void queue_dump(node *root);

node* create_node(){
    node *n = (node*) malloc(sizeof(node));
    n->next = NULL;
    return n;
}
void enqueue(queue* q, T data){
    node *n = create_node();
    n->data = data;
    n->next = NULL;
    if(q->front == NULL){
        q->front = n;
        q->end = n;
    }else{
        q->end->next = n;
        q->end = n;
    }
}
T dequeue(queue* q){
    if(q->front == NULL){
        T t = {NULL,-1};
        return t;
    }
    node *n = q->front;
    T data = n->data;
    q->front = q->front->next;
    free(n);
    return data;
}
void queue_dump(node *root){
    while(root != NULL){
        printf("%s ", root->data.data);
        root = root->next;
    }
    printf("\n");
}

#endif