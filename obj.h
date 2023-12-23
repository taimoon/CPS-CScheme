#ifndef obj_h
#define obj_h
#include<assert.h>
#include<stdint.h>

typedef enum Obj_Type {
    NUMBER,SYMBOL,PAIR,NIL,PRIM,CC,
} Value_Type;

typedef enum PRIM_OP {
    CAR,CDR,EQ,CONS,ATOM,
    ADD,SUB,MUL,DIV,PRINT,COLLECT,
} PRIM_OP;
typedef enum CONT_T {
    END,EVAL_EACH,IF,BEGIN,DEF,
} CONT_T;


typedef struct Value {
    int8_t moved;
    Value_Type type;
    union {
        char* symbol;
        int64_t value;
        struct {
            struct Value *car;
            struct Value *cdr;
        };
        PRIM_OP prim;
        struct Value *ref;
        CONT_T cc;
    };
} Value;

Value* car(Value *v){
    assert(v->type == PAIR);
    return v->car;
}

Value* cdr(Value *v){
    assert(v->type == PAIR);
    return v->cdr;
}
// r6rs suggests to provide up to 4 deep
#define caar(e) car(car(e))
#define cadr(e) car(cdr(e))
#define cdar(e) cdr(car(e))
#define cddr(e) cdr(cdr(e))
#define caddr(e) car(cddr(e))
#define caadr(e) car(cadr(e))
#define caaar(e) car(caar(e))
#define cadar(e) car(cdar(e))
#define cdddr(e) cdr(cddr(e))
#define cdadr(e) cdr(cadr(e))
#define cdaar(e) cdr(caar(e))
#define cddar(e) cdr(cdar(e))
#define caaddr(e) car(caddr(e))
#define caaadr(e) car(caadr(e))
#define caaaar(e) car(caaar(e))
#define caadar(e) car(cadar(e))
#define cadddr(e) car(cdddr(e))
#define cadadr(e) car(cdadr(e))
#define cadaar(e) car(cdaar(e))
#define caddar(e) car(cddar(e))
#define cdaddr(e) cdr(caddr(e))
#define cdaadr(e) cdr(caadr(e))
#define cdaaar(e) cdr(caaar(e))
#define cdadar(e) cdr(cadar(e))
#define cddddr(e) cdr(cdddr(e))
#define cddadr(e) cdr(cdadr(e))
#define cddaar(e) cdr(cdaar(e))
#define cdddar(e) cdr(cddar(e))

#endif