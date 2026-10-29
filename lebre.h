#ifndef LEBRE_H
#define LEBRE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <time.h>

typedef enum {
    T_FIM, T_DESCONHECIDO,
    T_VAR, T_CONS, T_INT, T_DECIMAL, T_TEXTO, T_BOOLEANO, T_LISTA, T_DATA,
    T_VERDADEIRO, T_FALSO,
    T_SE, T_SENAO, T_ENQUANTO, T_LINHA, T_LINHAS,
    T_FUNCAO, T_RETORNA, T_INCLUIR,
    T_MOSTRAR, T_CARREGAR, T_ADICIONAR, T_REMOVER, T_ORDENAR,
    T_LER_ARQUIVO, T_SALVAR_ARQUIVO, T_ALEATORIO, T_DIVIDIR,
    T_PARA_INT, T_PARA_DECIMAL, T_PARA_TEXTO,
    T_MEDIA, T_SOMA, T_MAXIMO, T_MINIMO, T_CONTAR,
    T_IDENTIFICADOR, T_SCREAM_SNAKE,
    T_NUM_INT, T_NUM_DEC, T_STRING,
    T_ATRIBUICAO, T_SOMA_OP, T_SUBTRACAO, T_MULTIPLICACAO, T_DIVISAO, T_MODULO, T_POTENCIA, T_RAIZ,
    T_IGUAL, T_IDENTICO, T_DIFERENTE, T_MAIOR, T_MENOR, T_MAIOR_IGUAL, T_MENOR_IGUAL,
    T_E, T_OU, T_INCREMENTO, T_DECREMENTO,
    T_ABRE_PAR, T_FECHA_PAR, T_ABRE_CHAVE, T_FECHA_CHAVE, T_ABRE_COL, T_FECHA_COL,
    T_PONTO_VIRGULA, T_VIRGULA, T_DOIS_PONTOS
} TokenType;

typedef struct {
    TokenType type;
    char value[256];
} Token;

typedef enum {
    AST_VAZIO, AST_PROGRAMA, AST_BLOCO,
    AST_DECLARACAO, AST_CONSTANTE, AST_ATRIBUICAO,
    AST_INDEX_ACESSO, AST_INDEX_ATRIBUICAO,
    AST_INCREMENTO, AST_DECREMENTO,
    AST_MOSTRAR, AST_SE, AST_ENQUANTO, AST_LINHA, AST_LINHAS,
    AST_FUNCAO_DEF, AST_CHAMADA_USER_FUNC, AST_CHAMADA_NATIVA, AST_RETORNA,
    AST_ADICIONAR, AST_REMOVER, AST_INCLUIR, AST_SALVAR_ARQUIVO, AST_DATA_DEF,
    AST_BINARIO, AST_LITERAL, AST_VARIAVEL
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;
    TokenType op;
    TokenType var_type;
    char name[128];
    struct ASTNode *left, *right, *cond, *body, *alt, *init, *step;
    struct ASTNode **args;
    int arg_count;
} ASTNode;

typedef enum {
    VAL_INT, VAL_DEC, VAL_TEXTO, VAL_BOOL, VAL_LISTA, VAL_FUNCAO
} ValueType;

struct LebreValue;

typedef struct {
    int tamanho;
    ValueType tipoItem;
    struct LebreValue* itens;
} LebreLista;

typedef struct LebreValue {
    ValueType type;
    union {
        int i;
        double d;
        char* s;
        int b;
        LebreLista l;
        ASTNode* func_def;
    } data;
} LebreValue;

typedef struct {
    char name[128];
    LebreValue val;
    int is_const;
} Variavel;

typedef struct Ambiente {
    Variavel vars[500];
    int count;
    struct Ambiente* pai;
} Ambiente;

extern Ambiente ambiente_global;
extern LebreValue global_return_val;
extern int flag_return;

void lancar_erro(const char* msg);
void lex(const char* src, Token* tokens);
ASTNode* parse(Token* tokens);
LebreValue get_var(Ambiente* env, const char* name);
void set_var(Ambiente* env, const char* name, LebreValue val, int is_const, int is_global);
LebreValue avalia_expressao(ASTNode* node, Ambiente* env);
void executa(ASTNode* node, Ambiente* env);

#endif