#ifndef eval_h
#define eval_h
#include"./parser.h"
#include"./obj.h"
#include"./memory.h"

int eq(Value *e1, Value *e2);
int is_tagged(Value *sym, char *s);
Value* value_of(Value* exp);
Value* create_bindings(Value* names, Value* vals, Value* env);
Value* extend_env(Value* params, Value* vals, Value* env);
void add_var_val(Value* name, Value *val, Value *env);
Value* apply_env(Value* sym,Value *env);
Value* create_prim(char* w, PRIM_OP d, Value* bindings);
Value* init_env();

void eval();
void apply();
void apply_prim();

// cont
void end_cont(){}
void eval_each_cont();
void if_cont();
void begin_cont();
void def_cont();

Value* assoc(Value* name, Value* pairs){
    while(pairs->type != NIL){
        assert(pairs->type == PAIR);
        Value* p = pairs->car;
        assert(p->type == PAIR);
        if(p->car->type != SYMBOL){}
        else if(p->car->symbol == name->symbol){
            return p;
        }
        pairs = pairs->cdr;
    }
    return pairs;
}
Value* create_bindings(Value* names, Value* vals, Value* env){
    if(names->type == NIL){
        return env;
    }else{
        return make_pair(make_pair(car(names),car(vals)),create_bindings(cdr(names),cdr(vals),env));
    }
}
Value* extend_env(Value* params, Value* vals, Value* env){
    return make_pair(create_bindings(params,vals,NIL_V),env);
}
void add_var_val(Value* name, Value *val, Value *env){
    assert(env->type != NIL);
    Value* pairs = car(env);
    while(pairs->type != NIL){
        assert(pairs->type == PAIR);
        Value* p = pairs->car;
        assert(p->type == PAIR);
        if(p->car->type != SYMBOL){}
        else if(p->car->symbol == name->symbol){
            p->cdr = val;
            return;
        }else if(pairs->cdr->type == NIL){
            pairs->cdr = make_pair(make_pair(name,val),make_nil());
            return;
        }
        pairs = pairs->cdr;
    }
}
Value* apply_env(Value* sym,Value *env){
    while(env->type != NIL){
        Value* tmp = assoc(sym,env->car);
        if(tmp->type == PAIR) return tmp->cdr;
        env = env->cdr;
    }
    printf("not bound! =%s\n", sym->symbol);
    assert(0);
}
Value* create_prim(char* w, PRIM_OP d, Value* bindings){
    w = intern(POOL,w);
    Value* prim = make_prim(d);
    Value* sym = make_symbol(intern(POOL,w));
    return make_pair(make_pair(sym,prim),bindings);
}
Value* init_env(){
    Value* bindings = NIL_V;
    ht* pool = POOL;
    bindings = create_prim("car",CAR,bindings);
    bindings = create_prim("cdr",CDR,bindings);
    bindings = create_prim("cons",CONS,bindings);
    bindings = create_prim("+",ADD,bindings);
    bindings = create_prim("*",MUL,bindings);
    bindings = create_prim("/",DIV,bindings);
    bindings = create_prim("-",SUB,bindings);
    bindings = create_prim("eq?",EQ,bindings);
    bindings = create_prim("print",PRINT,bindings);
    bindings = create_prim("collect",COLLECT,bindings);
    bindings = make_pair(bindings,NIL_V);
    return bindings;
}

void eval_each_cont(){
    // ar = make_pair(exps,make_pair(make_nil(),make_pair(make_nil(),make_pair(ENV,CONT))))
    EXP = CONT->car;
    // q-back = EXP->cdr->car
    // q-front = EXP->cdr->cdr->car
    if(EXP->cdr->car->type == NIL){
        EXP->cdr->cdr->car = make_pair(VAL,NIL_V);
        EXP->cdr->car = EXP->cdr->cdr->car;
    }else{
        EXP->cdr->car->cdr = make_pair(VAL,NIL_V);
        EXP->cdr->car = EXP->cdr->car->cdr;
    }
    if(EXP->car->type == NIL){
        VAL = EXP->cdr->cdr->car;
        CONT = EXP->cdr->cdr->cdr->cdr;
        EXP = VAL->car;
        VAL = VAL->cdr;
        return apply();
    }
    
    ENV = EXP->cdr->cdr->cdr->car;
    VAL = EXP->car->car;
    EXP->car = EXP->car->cdr;
    EXP = VAL;
    return eval();

}
void apply_cont(){
    allocate(16);
    assert(CONT->type == PAIR);
    assert(CONT->cdr->type == CC);
    switch (CONT->cdr->cc)
    {
    case END:
        break;
    case EVAL_EACH:
        return eval_each_cont();
    case IF:
        return if_cont();
    case BEGIN:
        return begin_cont();
    case DEF:
        return def_cont();
    default:
        assert(0);
        break;
    }
}

void if_cont(){
    // ar = make_pair(exps,make_pair(ENV,CONT))
    if(eq(VAL,FALSE_V)){
        EXP = (CONT->car)->car->cdr->car;
    }else{
        EXP = (CONT->car)->car->car;
    }
    ENV = CONT->car->cdr->car;
    CONT = CONT->car->cdr->cdr;
    return eval();
}
void begin_cont(){
    // ar = make_pair(exps,make_pair(ENV,CONT))
    ENV = (CONT->car)->cdr->car;
    EXP = (CONT->car)->car; // exps
    if(EXP->cdr->type == NIL){
        EXP = EXP->car;
        CONT = (CONT->car)->cdr->cdr;
        return eval();
    }
    (CONT->car)->car = EXP->cdr;
    EXP = EXP->car;
    return eval();
}
void def_cont(){
    // ar = make_pair(name,make_pair(ENV,CONT))
    EXP = (CONT->car)->car;
    ENV = (CONT->car)->cdr->car;
    add_var_val(EXP,VAL,ENV);
    CONT = (CONT->car)->cdr->cdr;
    VAL = make_nil();
    apply_cont();
}
Value* value_of(Value* exp){
    EXP = exp;
    ENV = init_env();
    CONT = make_cont(END);
    CONT = make_pair(make_nil(),CONT);
    VAL = make_nil();
    eval();
    assert(CONT->cdr->type == CC);
    while(CONT->cdr->cc != END){
        apply_cont();
        assert(CONT->cdr->type == CC);
    }
    return VAL;
}
void eval(){
    allocate(16);
    set_root();
    if(EXP->type == NUMBER || EXP->type == NIL){
        VAL = EXP;
        return; // return apply_cont();
    }
    if(EXP->type == SYMBOL){
        VAL = apply_env(EXP,ENV);
        return; // return apply_cont();
    }
    if(EXP->type != PAIR){
        printf("not a pair\n");
        assert(0);
    }
    if(is_tagged(car(EXP),"quote")){
        VAL = EXP->cdr->car;
        return; // return apply_cont();
    }
    if(is_tagged(car(EXP),"if")){
        // ar = make_pair(exps,make_pair(ENV,CONT))
        VAL = make_pair(ENV,CONT);
        VAL = make_pair(cddr(EXP),VAL);
        CONT = make_cont(IF);
        CONT = make_pair(VAL,CONT);
        EXP = cadr(EXP);
        return eval();
    }
    if(is_tagged(car(EXP),"begin")){
        // ar = make_pair(exps,make_pair(ENV,CONT))
        VAL = make_pair(ENV,CONT);
        VAL = make_pair(cddr(EXP),VAL);
        CONT = make_cont(BEGIN);
        CONT = make_pair(VAL,CONT);
        EXP = cadr(EXP);
        return eval();
    }
    if(is_tagged(car(EXP),"define")){
        // ar = make_pair(name,make_pair(ENV,CONT))
        VAL = make_pair(ENV,CONT);
        VAL = make_pair(cadr(EXP),VAL);
        
        CONT = make_cont(DEF);
        CONT = make_pair(VAL,CONT);
        EXP = caddr(EXP);
        return eval();
    }

    if(is_tagged(car(EXP),"lambda")){
        // (lambda params body)
        // ('closure params body . env)
        VAL = make_pair(caddr(EXP),ENV);
        VAL = make_pair(cadr(EXP),VAL);
        EXP = make_symbol("closure");
        VAL = make_pair(EXP,VAL);
        return;
    }

    // ar = make_pair(exps,make_pair(make_nil(),make_pair(make_nil(),make_pair(ENV,CONT))))
    VAL = make_pair(ENV,CONT);
    VAL = make_pair(make_nil(),VAL);
    VAL = make_pair(make_nil(),VAL);
    VAL = make_pair(EXP->cdr,VAL);
    CONT = make_cont(EVAL_EACH);
    CONT = make_pair(VAL,CONT);
    EXP = EXP->car;
    eval();
}
int is_tagged(Value *sym, char *s){
    if(sym->type == SYMBOL){
        return strcmp(sym->symbol,s) == 0;
    }
    return 0;
}
void apply(){
    allocate(16);
    if(EXP->type == PRIM){
        apply_prim();
        apply_cont();
    }else{
        // ('closure params body . env)
        ENV = EXP->cdr->cdr->cdr;
        ENV = extend_env(EXP->cdr->car,VAL,ENV);
        EXP = EXP->cdr->cdr->car;
        eval();
    }
}

int eq(Value *e1, Value *e2){
    if(e1->type!=e2->type)
        return 0;
    if(e1->type == NUMBER)
        return e1->value == e2->value;
    if(e1->type == SYMBOL){
        return e1->symbol == e2->symbol;
    }
    if(e1->type == NIL){
        return 1;
    }
    assert(0);
}
void apply_prim(){
    Value* vals = VAL;
    switch (EXP->prim)
    {
    case ADD:{
        int v = vals->car->value + vals->cdr->car->value;
        VAL = make_num(v);
        break;
    }
    case MUL:{
        int v = vals->car->value * vals->cdr->car->value;
        VAL = make_num(v);
        break;
    }
    case SUB:{
        int v = vals->car->value - vals->cdr->car->value;
        VAL = make_num(v);
        break;
    }
    case DIV:{
        int v = vals->car->value / vals->cdr->car->value;
        VAL = make_num(v);
        break;
    }
    case EQ:{
        VAL = make_num(eq(vals->car,vals->cdr->car));
        break;
    }
    case CAR:{
        VAL = car(car(vals));
        break;
    }
    case CDR:{
        VAL = cdr(car(vals));
        break;
    }
    case CONS:{
        VAL = make_pair(car(vals),cadr(vals));
        break;
    }
    case PRINT:{
        print_sexp(car(vals));printf("\n");
        VAL = NIL_V;
        break;
    }
    case COLLECT:{
        allocate(0);
        VAL = NIL_V;
        break;
    }
    default:
        printf("unknown primitive procedure %d\n",EXP->prim);
        assert(0);
        break;
    }
}
#endif