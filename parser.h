#ifndef parser_h
#define parser_h
#include<assert.h>
#include"./obj.h"
#include"./memory.h"
#include"./intern.h"
#include"./queue.h"
#define TOKEN_BUFFER_SIZE 512

typedef enum Token {
    OPEN_PAREN = '(',
    CLOSE_PAREN = ')',
    QUOTE = 'q',
    SYM = 's',
    DIGIT = '0',
    COMMENT = 'c',
    NONE = '_',
    ERROR = -1,
} Token;


Token next_token(FILE *fptr, char *buffer);
void lexing(FILE *fptr, queue *q);
Value* parse_list(queue *q);
Value* parse(queue *q);
Value* read(FILE *fptr);


int is_whitespace(char c){
    return c == ' ' || c == '\n' || c == '\t';
}

int is_open_paren(char c){
    return c == '(';
}

int is_close_paren(char c){
    return c == ')';
}

int is_terminating(char c){
    return is_whitespace(c) || is_open_paren(c) || is_close_paren(c) || c == '\0' || c == '\'' || c == ';';
}

Token next_token(FILE *fptr, char *buffer){
    Token tk = NONE;
    int pos = 0;
    char c;
    if(feof(fptr)) return tk;
    c = fgetc(fptr);
    // do nothing for whitespaces
    while(!feof(fptr) && is_whitespace(c))
        c = fgetc(fptr);
    // do nothing for comment block
    if(c == ';'){
        while(!feof(fptr) && c != '\n')
             c = fgetc(fptr);
        tk = COMMENT;
    }
    if(is_open_paren(c)){
        tk = OPEN_PAREN;
        buffer[0] = '(';
        buffer[1] = '\0';
        return tk;
    }
    if(is_close_paren(c)){
        tk = CLOSE_PAREN;
        buffer[0] = ')';
        buffer[1] = '\0';
        return tk;
    }
    if(c == '\''){
        tk = QUOTE;
        buffer[0] = '\'';
        buffer[1] = '\0';
        return tk;
    }
    
    // number
    if(!feof(fptr) && !is_terminating(c) && c >= '0' && c <= '9'){
        while(!feof(fptr) && !is_terminating(c) && c >= '0' && c <= '9'){
            buffer[pos] = c;
            pos = pos + 1;
            c = fgetc(fptr);
        }
        tk = DIGIT;
    }

    // symbol
    if(!feof(fptr) && !is_terminating(c)){
        while(!feof(fptr) &&  !is_terminating(c)){
            buffer[pos] = c;
            pos = pos + 1;
            c = fgetc(fptr);
        }
        tk = SYM;
    }

    ungetc(c,fptr);
    buffer[pos] = '\0';
    return tk;
}

void lexing(FILE *fptr, queue *q){
    char tag = '_';
    char buffer[TOKEN_BUFFER_SIZE];
    T ref;
    for(;!feof(fptr);){
        tag = next_token(fptr,buffer);
        if(tag != COMMENT && tag != NONE){
            ref.data = intern(POOL,buffer);
            ref.token = tag;
            enqueue(q,ref);
        }
    }
    char* dest = "quote";
    intern(POOL,dest);
}

Value* parse_list(queue *q){
    // peek first
    if(q->front == NULL) assert(0);
    T ref = q->front->data;
    Token tk = (Token) ref.token;
    char* data = ref.data;
    if(tk == CLOSE_PAREN){
        dequeue(q);
        return make_nil();
    }
    else{
        Value* left = parse(q);
        Value* right = parse_list(q);
        Value* res = make_pair(left,right);
        return res;
    }
}

Value* parse(queue *q){
    if(q->front == NULL) assert(0);
    T ref = dequeue(q);
    Token tk = (Token) ref.token;
    char* data = ref.data;
    if(tk == OPEN_PAREN){
        return parse_list(q);
    }
    if(tk == CLOSE_PAREN){
        return NIL_V;
    }
    if(tk == DIGIT){
        int v = atoi(data);
        return make_num(v);
    }
    if(tk == SYM){
        return make_symbol(data);
    }
    if(tk == QUOTE){
        char* ref = intern(POOL,"quote");
        Value* sym = parse(q);
        if(sym->type == NIL){
            return NIL_V;
        }else{
            Value* quote = make_symbol(ref);
            return make_pair(quote,make_pair(sym,make_nil()));
        }
    }
    assert(0);
}

Value* read(FILE *fptr){
    queue q = {NULL,NULL};
    Value* exp = NIL_V;
    lexing(fptr,&q);
    exp = parse(&q);
    assert(q.front == NULL);
    return exp;
}

#endif