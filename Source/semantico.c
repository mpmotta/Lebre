#include "lebre.h"

typedef struct {
    char name[128];
    int is_const;
    int is_class;
} Simbolo;

typedef struct Escopo {
    Simbolo simbolos[500];
    int count;
    struct Escopo* pai;
} Escopo;

Escopo* escopo_atual = NULL;

void iniciar_escopo() {
    Escopo* novo = malloc(sizeof(Escopo));
    novo->count = 0;
    novo->pai = escopo_atual;
    escopo_atual = novo;
}

void fechar_escopo() {
    if (escopo_atual) {
        Escopo* temp = escopo_atual;
        escopo_atual = escopo_atual->pai;
        free(temp);
    }
}

void declarar_simbolo(const char* name, int is_const, int is_class) {
    Escopo* target = escopo_atual;
    if (name[strlen(name)-1] == '!') {
        while (target->pai != NULL) {
            target = target->pai;
        }
    }
    
    for (int i = 0; i < target->count; i++) {
        if (strcmp(target->simbolos[i].name, name) == 0) {
            char msg[256];
            sprintf(msg, "%s ja declarado(a)", name);
            lancar_erro(msg);
        }
    }
    
    strcpy(target->simbolos[target->count].name, name);
    target->simbolos[target->count].is_const = is_const;
    target->simbolos[target->count].is_class = is_class;
    target->count++;
}

int buscar_simbolo(const char* name, int* is_const, int* is_class) {
    Escopo* curr = escopo_atual;
    while (curr != NULL) {
        for (int i = 0; i < curr->count; i++) {
            if (strcmp(curr->simbolos[i].name, name) == 0) {
                if (is_const) {
                    *is_const = curr->simbolos[i].is_const;
                }
                if (is_class) {
                    *is_class = curr->simbolos[i].is_class;
                }
                return 1;
            }
        }
        curr = curr->pai;
    }
    return 0;
}

void analisar_no(ASTNode* node);

void analisar_filhos(ASTNode* node) {
    if (!node) return;
    for (int i = 0; i < node->arg_count; i++) {
        analisar_no(node->args[i]);
    }
    analisar_no(node->left);
    analisar_no(node->right);
    analisar_no(node->cond);
    analisar_no(node->body);
    analisar_no(node->alt);
    analisar_no(node->init);
    analisar_no(node->step);
}

void analisar_no(ASTNode* node) {
    if (!node) return;

    if (node->type == AST_PROGRAMA || node->type == AST_BLOCO) {
        iniciar_escopo();
        for (int i = 0; i < node->arg_count; i++) {
            analisar_no(node->args[i]);
        }
        fechar_escopo();
        return;
    }
    
    if (node->type == AST_CLASSE_DEF) {
        declarar_simbolo(node->name, 0, 1);
        iniciar_escopo();
        analisar_no(node->body);
        fechar_escopo();
        return;
    }

    if (node->type == AST_DECLARACAO || node->type == AST_CONSTANTE) {
        analisar_no(node->right);
        declarar_simbolo(node->name, node->type == AST_CONSTANTE, 0);
        return;
    }
    
    if (node->type == AST_DATA_DEF) {
        analisar_no(node->right);
        declarar_simbolo(node->name, 0, 0);
        return;
    }

    if (node->type == AST_FUNCAO_DEF) {
        declarar_simbolo(node->name, 1, 0);
        iniciar_escopo();
        for (int i = 0; i < node->arg_count; i++) {
            declarar_simbolo(node->args[i]->name, 0, 0);
        }
        analisar_no(node->body);
        fechar_escopo();
        return;
    }
    
    if (node->type == AST_INSTANCIA) {
        for (int i = 0; i < node->arg_count; i++) {
            analisar_no(node->args[i]);
        }
        return;
    }

    if (node->type == AST_VARIAVEL || node->type == AST_INDEX_ACESSO || node->type == AST_INDEX_ATRIBUICAO || node->type == AST_ATRIBUICAO || node->type == AST_INCREMENTO || node->type == AST_DECREMENTO || node->type == AST_PROPRIEDADE_ACESSO || node->type == AST_PROPRIEDADE_ATRIBUICAO || node->type == AST_METODO_CHAMADA) {
        int is_const = 0;
        if (!buscar_simbolo(node->name, &is_const, NULL)) {
            char msg[256];
            sprintf(msg, "Uso de variavel nao declarada: %s", node->name);
            lancar_erro(msg);
        }
        if ((node->type == AST_ATRIBUICAO || node->type == AST_INDEX_ATRIBUICAO || node->type == AST_INCREMENTO || node->type == AST_DECREMENTO || node->type == AST_PROPRIEDADE_ATRIBUICAO) && is_const) {
            char msg[256];
            sprintf(msg, "Tentativa de reatribuir constante: %s", node->name);
            lancar_erro(msg);
        }
        
        if (node->type == AST_METODO_CHAMADA || node->type == AST_INDEX_ACESSO || node->type == AST_INDEX_ATRIBUICAO) {
            analisar_no(node->left);
            for (int i = 0; i < node->arg_count; i++) {
                analisar_no(node->args[i]);
            }
        }
        if (node->type == AST_ATRIBUICAO || node->type == AST_INDEX_ATRIBUICAO || node->type == AST_PROPRIEDADE_ATRIBUICAO) {
            analisar_no(node->right);
        }
        return;
    }
    
    if (node->type == AST_LINHAS) {
        analisar_no(node->right);
        iniciar_escopo();
        declarar_simbolo(node->name, 0, 0);
        analisar_no(node->body);
        fechar_escopo();
        return;
    }

    analisar_filhos(node);
}

void analisar_semantica(ASTNode* root) {
    escopo_atual = NULL;
    iniciar_escopo();
    analisar_no(root);
    fechar_escopo();
}