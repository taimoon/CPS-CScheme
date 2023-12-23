#ifndef memory_h
#define memory_h
#include<stdio.h>
#include<assert.h>
#include<stdlib.h>
#include"./obj.h"
#include"./intern.h"

static ht* POOL = NULL;
#define HEAP_CAP (1024)
static Value *heap;
static Value *free_heap;
static Value *root;

static Value *EXP;
static Value *ENV;
static Value *CONT;
static Value *VAL;

static Value* NIL_V;
static Value* QUOTE_V;
static Value* FALSE_V;


static int initialized = 0;
static size_t heap_size = 0;
Value* alloc_val();
void init_lisp_objs();
Value* make_num(int val);
Value* make_symbol(char* s);
Value* make_prim(PRIM_OP op);
Value* make_pair(Value* car, Value* cdr);
Value* make_nil();
Value* make_cont(CONT_T cc);
void allocate(size_t sz);
void trigger_gc();
Value* collect(Value* obj);
void print_sexp(Value *sexp);

void set_root(){
    root->car = EXP;
    root->cdr->car = ENV;
    root->cdr->cdr->car = CONT;
    root->cdr->cdr->cdr = VAL;
}
void reassign_reg(){
    EXP = root->car;
    ENV = root->cdr->car;
    CONT = root->cdr->cdr->car;
    VAL = root->cdr->cdr->cdr;
}

Value* alloc_val(){
    if(initialized == 0){
        heap = (Value*) calloc(HEAP_CAP,sizeof(Value));
        free_heap = (Value*) calloc(HEAP_CAP,sizeof(Value));
        heap_size = 0;
        initialized = 1;
        root = make_pair(CONT,VAL);
        root = make_pair(ENV,root);
        root = make_pair(EXP,root);
    }
    assert(heap_size < HEAP_CAP);
    Value* val = heap + heap_size;
    val->moved = 0;
    heap_size = heap_size + 1;
    return val;
}
void allocate(size_t sz){
    if(heap_size + sz >= HEAP_CAP){
        trigger_gc();
    }
    assert(heap_size + sz < HEAP_CAP);
}
void trigger_gc(){
    // gc-flip
    // size_t old =heap_size;
    Value *temp = heap;
    heap = free_heap;
    free_heap = temp;
    heap_size = 0;
    initialized = 0;
    init_lisp_objs();
    set_root();
    root = collect(root);
    // printf("old=%lld,new=%lld,reclaimed=%lld\n",old,heap_size,old-heap_size);
    reassign_reg();
}
Value* collect(Value* obj){
    // already-moved
    if(obj->moved == 1){
        return obj->ref;
    }
    obj->moved = 1;
    if(obj->type == NUMBER){
        obj->ref = make_num(obj->value);
        return obj->ref;
    }else if(obj->type == NIL){
        obj->ref = make_nil();
        return obj->ref;
    }else if(obj->type == PRIM){
        obj->ref = make_prim(obj->prim);
        return obj->ref;
    }else if(obj->type == CC){
        obj->ref = make_cont(obj->cc);
        return obj->ref;
    }else if(obj->type == SYMBOL){
        obj->ref= make_symbol(obj->symbol);
        return obj->ref;
    }else if(obj->type == PAIR){
        Value* left = collect(obj->car);
        Value* right = collect(obj->cdr);
        obj->ref = make_pair(left,right);
        return obj->ref;
    }else assert(0);
}
Value* make_num(int val){
    Value* n = alloc_val();
    n->type = NUMBER;
    n->value = val;
    return n;
}
Value* make_symbol(char* s){
    Value* sym = alloc_val();
    sym->type = SYMBOL;
    sym->symbol = s;
    return sym;
}
Value* make_prim(PRIM_OP op){
    Value* prim = alloc_val();
    prim->type = PRIM;
    prim->prim = op;
    return prim;
}
Value* make_pair(Value* car, Value* cdr){
    Value* p = alloc_val();
    p->type = PAIR;
    p->car = car;
    p->cdr = cdr;
    return p;
}
Value* make_nil(){
    return NIL_V;
}
Value* make_cont(CONT_T cc){
    Value* c = alloc_val();
    c->type = CC;
    c->cc = cc;
    return c;
}

void init_lisp_objs(){
    NIL_V = alloc_val();
    NIL_V->type = NIL;
    FALSE_V = make_num(0);
    QUOTE_V = make_symbol("quote");
}

void print_s(Value *sexp, int is_cdr){
    switch (sexp->type)
    {
    case NUMBER:
        printf("%d",sexp->value);
        break;
    case SYMBOL:
        printf("%s",sexp->symbol);
        break;
    case PAIR:
        if(is_cdr == 0)
            printf("(");
        else
            printf(" ");
        print_s(sexp->car,0);
        if(sexp->cdr->type != PAIR && sexp->cdr->type != NIL){
            printf(" . ");
            print_s(sexp->cdr,1);
            printf(")");
        }else
            print_s(sexp->cdr,1);
        break;
    case NIL:
        if(is_cdr == 1)
            printf(")");
        else
            printf("()");
        break;
    case PRIM:
        printf("<prim>");
        break;
    case CC:
        printf("<cont>");
        break;
    default:
        break;
    }

}
void print_sexp(Value *sexp){
    print_s(sexp,0);
}

#endif