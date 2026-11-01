#include "lebre.h"

Variavel* buscar_variavel(Ambiente* env, const char* name) {
    Ambiente* curr = env;
    while (curr != NULL) {
        for (int i = 0; i < curr->count; i++) {
            if (strcmp(curr->vars[i].name, name) == 0) {
                return &curr->vars[i];
            }
        }
        curr = curr->pai;
    }
    return NULL;
}

LebreValue get_var(Ambiente* env, const char* name) {
    Variavel* var = buscar_variavel(env, name);
    if (var) {
        return var->val;
    }
    char msg[256];
    sprintf(msg, "%s nao encontrada", name);
    lancar_erro(msg);
    LebreValue v; v.type = VAL_INT; v.data.i = 0;
    return v;
}

void set_var(Ambiente* env, const char* name, LebreValue val, int is_const, int is_global, TokenType visibilidade) {
    Ambiente* target = is_global ? &ambiente_global : env;
    
    Variavel* var = buscar_variavel(target, name);
    if (var) {
        if (var->is_const) {
            char msg[256];
            sprintf(msg, "Nao e permitido alterar a constante %s", name);
            lancar_erro(msg);
        }
        if (var->val.type != val.type && var->val.type != VAL_FUNCAO && var->val.type != VAL_CLASSE && var->val.type != VAL_OBJETO) {
            char msg[256];
            sprintf(msg, "Erro de tipagem na atribuicao da variavel %s", name);
            lancar_erro(msg);
        }
        var->val = val;
        return;
    }

    strcpy(target->vars[target->count].name, name);
    target->vars[target->count].val = val;
    target->vars[target->count].is_const = is_const;
    target->vars[target->count].visibilidade = visibilidade;
    target->count++;
}