#include "lebre.h"

LebreValue get_var(Ambiente* env, const char* name) {
    Ambiente* curr = env;
    while (curr != NULL) {
        for (int i = 0; i < curr->count; i++) {
            if (strcmp(curr->vars[i].name, name) == 0) {
                return curr->vars[i].val;
            }
        }
        curr = curr->pai;
    }
    char msg[256];
    sprintf(msg, "%s nao encontrada", name);
    lancar_erro(msg);
    LebreValue v; v.type = VAL_INT; v.data.i = 0;
    return v;
}

void set_var(Ambiente* env, const char* name, LebreValue val, int is_const, int is_global) {
    Ambiente* target = is_global ? &ambiente_global : env;
    
    Ambiente* curr = target;
    while (curr != NULL) {
        for (int i = 0; i < curr->count; i++) {
            if (strcmp(curr->vars[i].name, name) == 0) {
                if (curr->vars[i].is_const) {
                    char msg[256];
                    sprintf(msg, "Nao e permitido alterar a constante %s", name);
                    lancar_erro(msg);
                }
                if (curr->vars[i].val.type != val.type && curr->vars[i].val.type != VAL_FUNCAO && curr->vars[i].val.type != VAL_CLASSE) {
                    char msg[256];
                    sprintf(msg, "Erro de tipagem na atribuicao da variavel %s", name);
                    lancar_erro(msg);
                }
                curr->vars[i].val = val;
                return;
            }
        }
        curr = curr->pai;
    }

    strcpy(target->vars[target->count].name, name);
    target->vars[target->count].val = val;
    target->vars[target->count].is_const = is_const;
    target->count++;
}