#!/bin/bash

cat << 'EOF' > lebre.h
#ifndef LEBRE_H
#define LEBRE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <time.h>

typedef enum {
    T_VAR, T_CONS, T_INT, T_DECIMAL, T_TEXTO, T_BOOLEANO, T_LISTA, T_DATA,
    T_SE, T_SENAO, T_ENQUANTO, T_MOSTRAR, T_LINHA, T_LINHAS,
    T_CLASSE, T_CONSTRUTOR, T_METODO, T_RETORNA, T_VAZIO, T_NOVO, T_OBJETO,
    T_FUNCAO, T_CARREGAR, T_PARA_INT, T_PARA_DECIMAL, T_PARA_TEXTO, T_ORDENAR,
    T_INCLUIR, T_ADICIONAR, T_REMOVER, T_SALVAR_ARQUIVO, T_LER_ARQUIVO, T_DIVIDIR, T_ALEATORIO,
    T_IDENTIFICADOR, T_PASCAL_CASE, T_SCREAM_SNAKE,
    T_NUM_INT, T_NUM_DEC, T_STRING, T_VERDADEIRO, T_FALSO,
    T_ATRIBUICAO, T_IDENTICO, T_IGUAL, T_DIFERENTE, T_MAIOR, T_MENOR, T_MAIOR_IGUAL, T_MENOR_IGUAL,
    T_SOMA, T_SUBTRACAO, T_MULTIPLICACAO, T_DIVISAO, T_MODULO, T_POTENCIA, T_RAIZ,
    T_INCREMENTO, T_DECREMENTO,
    T_E, T_OU, T_NAO,
    T_ABRE_PAR, T_FECHA_PAR, T_ABRE_CHAVE, T_FECHA_CHAVE, T_ABRE_COL, T_FECHA_COL,
    T_VIRGULA, T_PONTO, T_DOIS_PONTOS, T_PONTO_VIRGULA, T_GLOBAL_MARK,
    T_MEDIA, T_SOMA_FUNC, T_MAXIMO, T_MINIMO, T_CONTAR, T_FIM
} TokenType;

typedef struct {
    TokenType type;
    char value[128];
} Token;

typedef enum {
    AST_PROGRAMA, AST_DECLARACAO, AST_CONSTANTE, AST_ATRIBUICAO,
    AST_MOSTRAR, AST_SE, AST_ENQUANTO, AST_LINHA, AST_LINHAS,
    AST_BINARIO, AST_LITERAL, AST_VARIAVEL, AST_CHAMADA_NATIVA,
    AST_DECLARA_LISTA, AST_CLASSE_DEF, AST_NOVO_OBJETO, AST_METODO_CALL,
    AST_BLOCO, AST_INCREMENTO, AST_DECREMENTO,
    AST_INDEX_ACESSO, AST_INDEX_ATRIBUICAO,
    AST_FUNCAO_DEF, AST_CHAMADA_USER_FUNC, AST_RETORNA, AST_DATA_DEF,
    AST_INCLUIR, AST_ADICIONAR, AST_REMOVER, AST_SALVAR_ARQUIVO, AST_VAZIO
} ASTNodeType;

typedef enum { VAL_INT, VAL_DEC, VAL_TEXTO, VAL_BOOL, VAL_LISTA, VAL_OBJETO, VAL_FUNCAO } ValType;

struct LebreValue;
struct ASTNode;

typedef struct {
    ValType tipoItem;
    int tamanho;
    struct LebreValue* itens;
} LebreLista;

typedef struct LebreValue {
    ValType type;
    union {
        int i;
        double d;
        char* s;
        int b;
        LebreLista l;
        struct Ambiente* obj;
        struct ASTNode* func_def;
    } data;
} LebreValue;

typedef struct ASTNode {
    ASTNodeType type;
    char name[128];
    TokenType op;
    struct ASTNode* left;
    struct ASTNode* right;
    struct ASTNode* cond;
    struct ASTNode* body;
    struct ASTNode* alt;
    struct ASTNode* init;
    struct ASTNode* step;
    struct ASTNode** args;
    int arg_count;
} ASTNode;

typedef struct Ambiente {
    char nomes[100][128];
    LebreValue valores[100];
    int read_only[100];
    int count;
    struct Ambiente* pai;
} Ambiente;

void lex(char* source, Token* tokens);
ASTNode* parse(Token* tokens);
void executa(ASTNode* node, Ambiente* env);
void set_var(Ambiente* env, char* name, LebreValue val, int is_const, int is_global);
LebreValue get_var(Ambiente* env, char* name);
LebreValue avalia_expressao(ASTNode* node, Ambiente* env);

extern Ambiente ambiente_global;
extern LebreValue global_return_val;
extern int flag_return;

#endif
EOF

cat << 'EOF' > main.c
#include "lebre.h"

Ambiente ambiente_global;
LebreValue global_return_val;
int flag_return = 0;

int main(int argc, char** argv) {
    srand((unsigned int)time(NULL));
    ambiente_global.count = 0;
    ambiente_global.pai = NULL;

    if (argc == 1) {
        char linha[2048];
        printf("Interpretador Lebre v1.0\n");
        printf("Digite 'sair' para encerrar o terminal.\n");
        
        while (1) {
            printf("lebre> ");
            if (!fgets(linha, sizeof(linha), stdin)) {
                break;
            }
            if (strncmp(linha, "sair", 4) == 0) {
                break;
            }
            Token* tokens = malloc(sizeof(Token) * 10000);
            lex(linha, tokens);
            ASTNode* root = parse(tokens);
            executa(root, &ambiente_global);
            free(tokens);
        }
        return 0;
    }

    FILE* file = fopen(argv[1], "rb");
    if (!file) {
        printf("Erro ao abrir o arquivo %s\n", argv[1]);
        return 1;
    }
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* buffer = malloc(length + 1);
    fread(buffer, 1, length, file);
    buffer[length] = '\0';
    fclose(file);

    Token* tokens = malloc(sizeof(Token) * 10000);
    lex(buffer, tokens);
    ASTNode* root = parse(tokens);
    executa(root, &ambiente_global);

    free(tokens);
    free(buffer);
    return 0;
}
EOF

cat << 'EOF' > lexer.c
#include "lebre.h"

int match_word(const char* str, const char* word) { return strcmp(str, word) == 0; }
int is_scream_snake(const char* str) {
    if (!isupper(str[0])) return 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isupper(str[i]) && str[i] != '_' && !isdigit(str[i])) return 0;
    }
    return 1;
}
int is_pascal_case(const char* str) { return isupper(str[0]); }
int is_camel_case(const char* str) { return islower(str[0]); }

void lex(char* source, Token* tokens) {
    int i = 0, t = 0;
    while (source[i] != '\0') {
        if (isspace(source[i])) { i++; continue; }
        if (source[i] == '/' && source[i+1] == '/') {
            while (source[i] != '\n' && source[i] != '\0') i++;
            continue;
        }
        if (source[i] == '+' && source[i+1] == '+') { tokens[t].type = T_INCREMENTO; strcpy(tokens[t].value, "++"); t++; i+=2; continue; }
        if (source[i] == '-' && source[i+1] == '-') { tokens[t].type = T_DECREMENTO; strcpy(tokens[t].value, "--"); t++; i+=2; continue; }
        if (source[i] == '=' && source[i+1] == '=' && source[i+2] == '=') { tokens[t].type = T_IDENTICO; strcpy(tokens[t].value, "==="); t++; i+=3; continue; }
        if (source[i] == '=' && source[i+1] == '=') { tokens[t].type = T_IGUAL; strcpy(tokens[t].value, "=="); t++; i+=2; continue; }
        if (source[i] == '!' && source[i+1] == '=') { tokens[t].type = T_DIFERENTE; strcpy(tokens[t].value, "!="); t++; i+=2; continue; }
        if (source[i] == '>' && source[i+1] == '=') { tokens[t].type = T_MAIOR_IGUAL; strcpy(tokens[t].value, ">="); t++; i+=2; continue; }
        if (source[i] == '<' && source[i+1] == '=') { tokens[t].type = T_MENOR_IGUAL; strcpy(tokens[t].value, "<="); t++; i+=2; continue; }
        
        char c = source[i];
        if (c == '=') { tokens[t].type = T_ATRIBUICAO; strcpy(tokens[t].value, "="); t++; i++; continue; }
        if (c == '+') { tokens[t].type = T_SOMA; strcpy(tokens[t].value, "+"); t++; i++; continue; }
        if (c == '-') { tokens[t].type = T_SUBTRACAO; strcpy(tokens[t].value, "-"); t++; i++; continue; }
        if (c == '*') { tokens[t].type = T_MULTIPLICACAO; strcpy(tokens[t].value, "*"); t++; i++; continue; }
        if (c == '/') { tokens[t].type = T_DIVISAO; strcpy(tokens[t].value, "/"); t++; i++; continue; }
        if (c == '%') { tokens[t].type = T_MODULO; strcpy(tokens[t].value, "%"); t++; i++; continue; }
        if (c == '^') { tokens[t].type = T_POTENCIA; strcpy(tokens[t].value, "^"); t++; i++; continue; }
        if (c == '$') { tokens[t].type = T_RAIZ; strcpy(tokens[t].value, "$"); t++; i++; continue; }
        if (c == '(') { tokens[t].type = T_ABRE_PAR; strcpy(tokens[t].value, "("); t++; i++; continue; }
        if (c == ')') { tokens[t].type = T_FECHA_PAR; strcpy(tokens[t].value, ")"); t++; i++; continue; }
        if (c == '{') { tokens[t].type = T_ABRE_CHAVE; strcpy(tokens[t].value, "{"); t++; i++; continue; }
        if (c == '}') { tokens[t].type = T_FECHA_CHAVE; strcpy(tokens[t].value, "}"); t++; i++; continue; }
        if (c == '[') { tokens[t].type = T_ABRE_COL; strcpy(tokens[t].value, "["); t++; i++; continue; }
        if (c == ']') { tokens[t].type = T_FECHA_COL; strcpy(tokens[t].value, "]"); t++; i++; continue; }
        if (c == ',') { tokens[t].type = T_VIRGULA; strcpy(tokens[t].value, ","); t++; i++; continue; }
        if (c == ':') { tokens[t].type = T_DOIS_PONTOS; strcpy(tokens[t].value, ":"); t++; i++; continue; }
        if (c == ';') { tokens[t].type = T_PONTO_VIRGULA; strcpy(tokens[t].value, ";"); t++; i++; continue; }
        if (c == '.') { tokens[t].type = T_PONTO; strcpy(tokens[t].value, "."); t++; i++; continue; }

        if (c == '"') {
            i++; int j = 0;
            while (source[i] != '"' && source[i] != '\0') tokens[t].value[j++] = source[i++];
            tokens[t].value[j] = '\0'; tokens[t].type = T_STRING;
            i++; t++; continue;
        }

        if (isdigit(c)) {
            int j = 0, is_dec = 0;
            while (isdigit(source[i]) || source[i] == '.') {
                if (source[i] == '.') is_dec = 1;
                tokens[t].value[j++] = source[i++];
            }
            tokens[t].value[j] = '\0'; tokens[t].type = is_dec ? T_NUM_DEC : T_NUM_INT;
            t++; continue;
        }

        if (c == '#') {
            i++; int j = 0;
            while (isalnum(source[i]) || source[i] == '_') tokens[t].value[j++] = source[i++];
            if (source[i] == '!') { tokens[t].value[j++] = '!'; i++; }
            tokens[t].value[j] = '\0';
            if (is_scream_snake(tokens[t].value)) tokens[t].type = T_SCREAM_SNAKE;
            else if (is_camel_case(tokens[t].value)) tokens[t].type = T_IDENTIFICADOR;
            else { printf("Erro: Nomenclatura invalida: %s\n", tokens[t].value); exit(1); }
            t++; continue;
        }

        if (isalpha(c)) {
            int j = 0;
            while (isalnum(source[i]) || source[i] == '_') tokens[t].value[j++] = source[i++];
            tokens[t].value[j] = '\0';
            if (match_word(tokens[t].value, "var")) tokens[t].type = T_VAR;
            else if (match_word(tokens[t].value, "cons")) tokens[t].type = T_CONS;
            else if (match_word(tokens[t].value, "int")) tokens[t].type = T_INT;
            else if (match_word(tokens[t].value, "decimal")) tokens[t].type = T_DECIMAL;
            else if (match_word(tokens[t].value, "texto")) tokens[t].type = T_TEXTO;
            else if (match_word(tokens[t].value, "booleano")) tokens[t].type = T_BOOLEANO;
            else if (match_word(tokens[t].value, "lista")) tokens[t].type = T_LISTA;
            else if (match_word(tokens[t].value, "data")) tokens[t].type = T_DATA;
            else if (match_word(tokens[t].value, "VERDADEIRO")) tokens[t].type = T_VERDADEIRO;
            else if (match_word(tokens[t].value, "FALSO")) tokens[t].type = T_FALSO;
            else if (match_word(tokens[t].value, "se")) tokens[t].type = T_SE;
            else if (match_word(tokens[t].value, "senao")) tokens[t].type = T_SENAO;
            else if (match_word(tokens[t].value, "enquanto")) tokens[t].type = T_ENQUANTO;
            else if (match_word(tokens[t].value, "mostrar")) tokens[t].type = T_MOSTRAR;
            else if (match_word(tokens[t].value, "linha")) tokens[t].type = T_LINHA;
            else if (match_word(tokens[t].value, "linhas")) tokens[t].type = T_LINHAS;
            else if (match_word(tokens[t].value, "funcao")) tokens[t].type = T_FUNCAO;
            else if (match_word(tokens[t].value, "carregar")) tokens[t].type = T_CARREGAR;
            else if (match_word(tokens[t].value, "paraInt")) tokens[t].type = T_PARA_INT;
            else if (match_word(tokens[t].value, "paraDecimal")) tokens[t].type = T_PARA_DECIMAL;
            else if (match_word(tokens[t].value, "paraTexto")) tokens[t].type = T_PARA_TEXTO;
            else if (match_word(tokens[t].value, "ordenar")) tokens[t].type = T_ORDENAR;
            else if (match_word(tokens[t].value, "incluir")) tokens[t].type = T_INCLUIR;
            else if (match_word(tokens[t].value, "adicionar")) tokens[t].type = T_ADICIONAR;
            else if (match_word(tokens[t].value, "remover")) tokens[t].type = T_REMOVER;
            else if (match_word(tokens[t].value, "salvarArquivo")) tokens[t].type = T_SALVAR_ARQUIVO;
            else if (match_word(tokens[t].value, "lerArquivo")) tokens[t].type = T_LER_ARQUIVO;
            else if (match_word(tokens[t].value, "dividir")) tokens[t].type = T_DIVIDIR;
            else if (match_word(tokens[t].value, "aleatorio")) tokens[t].type = T_ALEATORIO;
            else if (match_word(tokens[t].value, "classe")) tokens[t].type = T_CLASSE;
            else if (match_word(tokens[t].value, "novo")) tokens[t].type = T_NOVO;
            else if (match_word(tokens[t].value, "Objeto")) tokens[t].type = T_OBJETO;
            else if (match_word(tokens[t].value, "retorna")) tokens[t].type = T_RETORNA;
            else if (match_word(tokens[t].value, "E")) tokens[t].type = T_E;
            else if (match_word(tokens[t].value, "OU")) tokens[t].type = T_OU;
            else {
                if (is_pascal_case(tokens[t].value)) tokens[t].type = T_PASCAL_CASE;
                else if (is_camel_case(tokens[t].value)) tokens[t].type = T_IDENTIFICADOR;
            }
            t++; continue;
        }
        i++;
    }
    tokens[t].type = T_FIM;
}
EOF

cat << 'EOF' > parser.c
#include "lebre.h"

int pos = 0; Token* tkns;

ASTNode* create_node(ASTNodeType type) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = type; node->left = node->right = node->cond = node->body = node->alt = node->init = node->step = NULL;
    node->args = malloc(sizeof(ASTNode*) * 50); node->arg_count = 0;
    return node;
}

int get_precedence(TokenType type) {
    switch(type) {
        case T_OU: return 1;
        case T_E: return 2;
        case T_IDENTICO: case T_IGUAL: case T_DIFERENTE: return 3;
        case T_MAIOR: case T_MENOR: case T_MAIOR_IGUAL: case T_MENOR_IGUAL: return 4;
        case T_SOMA: case T_SUBTRACAO: return 5;
        case T_MULTIPLICACAO: case T_DIVISAO: case T_MODULO: return 6;
        case T_POTENCIA: case T_RAIZ: return 7;
        default: return 0;
    }
}

ASTNode* parse_expression();

ASTNode* parse_primary() {
    ASTNode* node = create_node(AST_LITERAL);
    if (tkns[pos].type == T_CARREGAR) {
        node->type = AST_CHAMADA_NATIVA; node->op = tkns[pos].type; pos++;
        if(tkns[pos].type == T_ABRE_PAR) pos++; if(tkns[pos].type == T_FECHA_PAR) pos++;
        return node;
    }
    if (tkns[pos].type == T_PARA_INT || tkns[pos].type == T_PARA_DECIMAL || tkns[pos].type == T_PARA_TEXTO || tkns[pos].type == T_ORDENAR || tkns[pos].type == T_LER_ARQUIVO || tkns[pos].type == T_ALEATORIO || tkns[pos].type == T_DIVIDIR) {
        node->type = AST_CHAMADA_NATIVA; node->op = tkns[pos].type; pos++;
        if(tkns[pos].type == T_ABRE_PAR) pos++;
        while (tkns[pos].type != T_FECHA_PAR && tkns[pos].type != T_FIM) {
            if (tkns[pos].type == T_VIRGULA) pos++;
            else node->args[node->arg_count++] = parse_expression();
        }
        if(tkns[pos].type == T_FECHA_PAR) pos++;
        return node;
    }
    if (tkns[pos].type == T_NUM_INT || tkns[pos].type == T_NUM_DEC || tkns[pos].type == T_STRING || tkns[pos].type == T_VERDADEIRO || tkns[pos].type == T_FALSO) {
        strcpy(node->name, tkns[pos].value); node->op = tkns[pos].type; pos++;
        return node;
    }
    if (tkns[pos].type == T_IDENTIFICADOR || tkns[pos].type == T_SCREAM_SNAKE) {
        strcpy(node->name, tkns[pos].value); pos++;
        if (tkns[pos].type == T_ABRE_COL) {
            node->type = AST_INDEX_ACESSO; pos++;
            node->left = parse_expression();
            if (tkns[pos].type == T_FECHA_COL) pos++;
            return node;
        }
        if (tkns[pos].type == T_ABRE_PAR) {
            node->type = AST_CHAMADA_USER_FUNC; pos++;
            while (tkns[pos].type != T_FECHA_PAR) {
                if (tkns[pos].type == T_VIRGULA) pos++;
                else node->args[node->arg_count++] = parse_expression();
            }
            pos++;
            return node;
        }
        node->type = AST_VARIAVEL;
        return node;
    }
    if (tkns[pos].type == T_ABRE_PAR) {
        pos++; ASTNode* expr = parse_expression();
        if (tkns[pos].type == T_FECHA_PAR) pos++;
        return expr;
    }
    return node;
}

ASTNode* parse_binop(ASTNode* left, int min_prec) {
    while (1) {
        int prec = get_precedence(tkns[pos].type);
        if (prec < min_prec || prec == 0) break;
        TokenType op = tkns[pos].type; pos++;
        ASTNode* right = parse_primary();
        int next_prec = get_precedence(tkns[pos].type);
        if (prec < next_prec) right = parse_binop(right, prec + 1);
        ASTNode* new_node = create_node(AST_BINARIO);
        new_node->op = op; new_node->left = left; new_node->right = right;
        left = new_node;
    }
    return left;
}

ASTNode* parse_expression() { return parse_binop(parse_primary(), 1); }

ASTNode* parse_bloco() {
    ASTNode* bloco = create_node(AST_BLOCO);
    if (tkns[pos].type == T_ABRE_CHAVE) pos++;
    while (tkns[pos].type != T_FECHA_CHAVE && tkns[pos].type != T_FIM) {
        bloco->args[bloco->arg_count++] = parse_expression(); 
        pos++;
    }
    if (tkns[pos].type == T_FECHA_CHAVE) pos++;
    return bloco;
}

ASTNode* parse_statement() {
    ASTNode* node = NULL;
    if (tkns[pos].type == T_INCLUIR) {
        node = create_node(AST_INCLUIR); pos++;
        if (tkns[pos].type == T_STRING) { strcpy(node->name, tkns[pos].value); pos++; }
        if (tkns[pos].type == T_PONTO_VIRGULA) pos++;
        return node;
    }
    if (tkns[pos].type == T_ADICIONAR) {
        node = create_node(AST_ADICIONAR); pos++;
        if (tkns[pos].type == T_ABRE_PAR) pos++;
        strcpy(node->name, tkns[pos].value); pos++; 
        if (tkns[pos].type == T_VIRGULA) pos++;
        node->right = parse_expression();
        if (tkns[pos].type == T_FECHA_PAR) pos++;
        if (tkns[pos].type == T_PONTO_VIRGULA) pos++;
        return node;
    }
    if (tkns[pos].type == T_REMOVER) {
        node = create_node(AST_REMOVER); pos++;
        if (tkns[pos].type == T_ABRE_PAR) pos++;
        strcpy(node->name, tkns[pos].value); pos++; 
        if (tkns[pos].type == T_VIRGULA) pos++;
        node->right = parse_expression();
        if (tkns[pos].type == T_FECHA_PAR) pos++;
        if (tkns[pos].type == T_PONTO_VIRGULA) pos++;
        return node;
    }
    if (tkns[pos].type == T_SALVAR_ARQUIVO) {
        node = create_node(AST_SALVAR_ARQUIVO); pos++;
        if (tkns[pos].type == T_ABRE_PAR) pos++;
        node->left = parse_expression(); 
        if (tkns[pos].type == T_VIRGULA) pos++;
        node->right = parse_expression(); 
        if (tkns[pos].type == T_FECHA_PAR) pos++;
        if (tkns[pos].type == T_PONTO_VIRGULA) pos++;
        return node;
    }
    if (tkns[pos].type == T_RETORNA) {
        node = create_node(AST_RETORNA); pos++;
        node->right = parse_expression();
        if (tkns[pos].type == T_PONTO_VIRGULA) pos++;
        return node;
    }
    if (tkns[pos].type == T_FUNCAO) {
        node = create_node(AST_FUNCAO_DEF); pos++; pos++;
        strcpy(node->name, tkns[pos].value); pos++;
        if (tkns[pos].type == T_ABRE_PAR) pos++;
        while (tkns[pos].type != T_FECHA_PAR) {
            if (tkns[pos].type == T_VIRGULA || tkns[pos].type == T_INT || tkns[pos].type == T_TEXTO || tkns[pos].type == T_DECIMAL) pos++;
            else { ASTNode* arg = create_node(AST_VARIAVEL); strcpy(arg->name, tkns[pos].value); node->args[node->arg_count++] = arg; pos++; }
        }
        pos++; node->body = parse_bloco();
        return node;
    }
    if (tkns[pos].type == T_VAR || tkns[pos].type == T_CONS) {
        int is_const = (tkns[pos].type == T_CONS); pos++;
        if (tkns[pos].type == T_DATA) {
            node = create_node(AST_DATA_DEF); pos++;
            strcpy(node->name, tkns[pos].value); pos++;
            if (tkns[pos].type == T_ABRE_PAR) pos++;
            node->right = parse_expression();
            if (tkns[pos].type == T_FECHA_PAR) pos++;
            if (tkns[pos].type == T_PONTO_VIRGULA) pos++;
            return node;
        }
        node = create_node(is_const ? AST_CONSTANTE : AST_DECLARACAO); pos++;
        strcpy(node->name, tkns[pos].value); pos++;
        if (tkns[pos].type == T_PONTO_VIRGULA) { pos++; return node; }
    }
    if (tkns[pos].type == T_IDENTIFICADOR || tkns[pos].type == T_SCREAM_SNAKE) {
        char temp_name[128]; strcpy(temp_name, tkns[pos].value); pos++;
        if (tkns[pos].type == T_ABRE_COL) {
            node = create_node(AST_INDEX_ATRIBUICAO); strcpy(node->name, temp_name); pos++;
            node->left = parse_expression();
            if (tkns[pos].type == T_FECHA_COL) pos++;
            if (tkns[pos].type == T_ATRIBUICAO) { pos++; node->right = parse_expression(); }
            if (tkns[pos].type == T_PONTO_VIRGULA) pos++;
            return node;
        }
        if (tkns[pos].type == T_INCREMENTO || tkns[pos].type == T_DECREMENTO) {
            node = create_node(tkns[pos].type == T_INCREMENTO ? AST_INCREMENTO : AST_DECREMENTO);
            strcpy(node->name, temp_name); pos++;
            if (tkns[pos].type == T_PONTO_VIRGULA) pos++;
            return node;
        }
        if (tkns[pos].type == T_ATRIBUICAO) {
            node = create_node(AST_ATRIBUICAO); strcpy(node->name, temp_name); pos++;
            node->right = parse_expression();
            if (tkns[pos].type == T_PONTO_VIRGULA) pos++;
            return node;
        }
        if (tkns[pos].type == T_ABRE_PAR) {
            pos--; return parse_expression();
        }
    }
    if (tkns[pos].type == T_MOSTRAR) {
        node = create_node(AST_MOSTRAR); pos++;
        if (tkns[pos].type == T_ABRE_PAR) pos++;
        node->right = parse_expression();
        if (tkns[pos].type == T_FECHA_PAR) pos++;
        if (tkns[pos].type == T_PONTO_VIRGULA) pos++;
        return node;
    }
    pos++; return create_node(AST_VAZIO);
}

ASTNode* parse(Token* tokens) {
    tkns = tokens; pos = 0;
    ASTNode* root = create_node(AST_PROGRAMA);
    while (tkns[pos].type != T_FIM) root->args[root->arg_count++] = parse_statement();
    return root;
}
EOF

cat << 'EOF' > memoria.c
#include "lebre.h"

void set_var(Ambiente* env, char* name, LebreValue val, int is_const, int is_global) {
    Ambiente* target = is_global ? &ambiente_global : env;
    for (int i = 0; i < target->count; i++) {
        if (strcmp(target->nomes[i], name) == 0) {
            if (target->read_only[i]) { printf("Erro: Constante %s\n", name); exit(1); }
            target->valores[i] = val;
            return;
        }
    }
    strcpy(target->nomes[target->count], name);
    target->valores[target->count] = val; target->read_only[target->count] = is_const;
    target->count++;
}

LebreValue get_var(Ambiente* env, char* name) {
    Ambiente* current = env;
    while (current != NULL) {
        for (int i = 0; i < current->count; i++) {
            if (strcmp(current->nomes[i], name) == 0) return current->valores[i];
        }
        current = current->pai;
    }
    printf("Erro: %s nao encontrada\n", name); exit(1);
}
EOF

cat << 'EOF' > executor.c
#include "lebre.h"

void formatar_data(char* dest, const char* fmt) {
    time_t t = time(NULL); struct tm *tm_info = localtime(&t);
    char buffer[256] = "", temp[32]; const char* p = fmt;
    const char* dias[] = {"Domingo", "Segunda-feira", "Terca-feira", "Quarta-feira", "Quinta-feira", "Sexta-feira", "Sabado"};
    const char* meses[] = {"Janeiro", "Fevereiro", "Marco", "Abril", "Maio", "Junho", "Julho", "Agosto", "Setembro", "Outubro", "Novembro", "Dezembro"};
    
    while(*p) {
        if (strncmp(p, "dd", 2) == 0) { sprintf(temp, "%02d", tm_info->tm_mday); strcat(buffer, temp); p+=2; }
        else if (strncmp(p, "mm", 2) == 0) { sprintf(temp, "%02d", tm_info->tm_mon + 1); strcat(buffer, temp); p+=2; }
        else if (strncmp(p, "yyyy", 4) == 0) { sprintf(temp, "%04d", tm_info->tm_year + 1900); strcat(buffer, temp); p+=4; }
        else if (strncmp(p, "h", 1) == 0) { sprintf(temp, "%02d", tm_info->tm_hour); strcat(buffer, temp); p+=1; }
        else if (strncmp(p, "i", 1) == 0) { sprintf(temp, "%02d", tm_info->tm_min); strcat(buffer, temp); p+=1; }
        else if (strncmp(p, "s", 1) == 0) { sprintf(temp, "%02d", tm_info->tm_sec); strcat(buffer, temp); p+=1; }
        else if (strncmp(p, "S", 1) == 0) { strcat(buffer, dias[tm_info->tm_wday]); p+=1; }
        else if (strncmp(p, "M", 1) == 0) { strcat(buffer, meses[tm_info->tm_mon]); p+=1; }
        else { int len = strlen(buffer); buffer[len] = *p; buffer[len+1] = '\0'; p++; }
    }
    strcpy(dest, buffer);
}

LebreValue avalia_expressao(ASTNode* node, Ambiente* env) {
    LebreValue val; val.type = VAL_INT; val.data.i = 0;
    if (!node) return val;
    if (node->type == AST_LITERAL) {
        if (node->op == T_NUM_INT) { val.type = VAL_INT; val.data.i = atoi(node->name); }
        else if (node->op == T_NUM_DEC) { val.type = VAL_DEC; val.data.d = atof(node->name); }
        else if (node->op == T_STRING) { val.type = VAL_TEXTO; val.data.s = strdup(node->name); }
        else if (node->op == T_VERDADEIRO) { val.type = VAL_BOOL; val.data.b = 1; }
        else if (node->op == T_FALSO) { val.type = VAL_BOOL; val.data.b = 0; }
        return val;
    }
    if (node->type == AST_VARIAVEL) return get_var(env, node->name);
    if (node->type == AST_INDEX_ACESSO) {
        LebreValue lista = get_var(env, node->name);
        LebreValue idx = avalia_expressao(node->left, env);
        if (lista.type == VAL_LISTA && idx.type == VAL_INT && idx.data.i >= 0 && idx.data.i < lista.data.l.tamanho) {
            return lista.data.l.itens[idx.data.i];
        }
        return val;
    }
    if (node->type == AST_CHAMADA_USER_FUNC) {
        LebreValue func = get_var(env, node->name);
        Ambiente* func_env = malloc(sizeof(Ambiente));
        func_env->count = 0; func_env->pai = &ambiente_global;
        for(int i=0; i < node->arg_count; i++) set_var(func_env, func.data.func_def->args[i]->name, avalia_expressao(node->args[i], env), 0, 0);
        executa(func.data.func_def->body, func_env);
        LebreValue res = global_return_val; flag_return = 0; free(func_env); return res;
    }
    if (node->type == AST_CHAMADA_NATIVA) {
        if (node->op == T_CARREGAR) {
            char buf[1024]; if (fgets(buf, sizeof(buf), stdin)) buf[strcspn(buf, "\n")] = '\0';
            val.type = VAL_TEXTO; val.data.s = strdup(buf); return val;
        }
        if (node->op == T_LER_ARQUIVO) {
            LebreValue arq = avalia_expressao(node->args[0], env);
            if (arq.type == VAL_TEXTO) {
                FILE* f = fopen(arq.data.s, "rb");
                if (f) {
                    fseek(f, 0, SEEK_END); long l = ftell(f); fseek(f, 0, SEEK_SET);
                    char* b = malloc(l + 1); fread(b, 1, l, f); b[l] = '\0'; fclose(f);
                    val.type = VAL_TEXTO; val.data.s = b; return val;
                }
            }
            val.type = VAL_TEXTO; val.data.s = strdup(""); return val;
        }
        if (node->op == T_ALEATORIO) {
            LebreValue min = avalia_expressao(node->args[0], env);
            LebreValue max = avalia_expressao(node->args[1], env);
            val.type = VAL_INT; val.data.i = (rand() % (max.data.i - min.data.i + 1)) + min.data.i;
            return val;
        }
        if (node->op == T_DIVIDIR) {
            LebreValue txt = avalia_expressao(node->args[0], env);
            LebreValue sep = avalia_expressao(node->args[1], env);
            if (txt.type == VAL_TEXTO && sep.type == VAL_TEXTO) {
                char* str = strdup(txt.data.s); char* token = strtok(str, sep.data.s);
                LebreValue* itens = NULL; int tam = 0;
                while(token != NULL) {
                    tam++; itens = realloc(itens, sizeof(LebreValue) * tam);
                    itens[tam-1].type = VAL_TEXTO; itens[tam-1].data.s = strdup(token);
                    token = strtok(NULL, sep.data.s);
                }
                free(str);
                val.type = VAL_LISTA; val.data.l.tamanho = tam; val.data.l.itens = itens; val.data.l.tipoItem = VAL_TEXTO;
                return val;
            }
        }
        if (node->op == T_PARA_INT || node->op == T_PARA_DECIMAL || node->op == T_PARA_TEXTO) {
            LebreValue a = avalia_expressao(node->args[0], env);
            if (node->op == T_PARA_INT) { val.type = VAL_INT; val.data.i = (a.type == VAL_TEXTO) ? atoi(a.data.s) : (a.type == VAL_DEC ? (int)a.data.d : a.data.i); }
            if (node->op == T_PARA_DECIMAL) { val.type = VAL_DEC; val.data.d = (a.type == VAL_TEXTO) ? atof(a.data.s) : (a.type == VAL_INT ? (double)a.data.i : a.data.d); }
            if (node->op == T_PARA_TEXTO) {
                char b[64]; if (a.type == VAL_INT) sprintf(b, "%d", a.data.i); else if (a.type == VAL_DEC) sprintf(b, "%f", a.data.d); else strcpy(b, a.data.s);
                val.type = VAL_TEXTO; val.data.s = strdup(b);
            }
            return val;
        }
        if (node->op == T_ORDENAR) {
            LebreValue lista = get_var(env, node->args[0]->name);
            if (lista.type == VAL_LISTA) {
                for(int i=0; i<lista.data.l.tamanho-1; i++) {
                    for(int j=0; j<lista.data.l.tamanho-i-1; j++) {
                        LebreValue a = lista.data.l.itens[j], b = lista.data.l.itens[j+1]; int swap = 0;
                        if(a.type == VAL_INT && a.data.i > b.data.i) swap = 1;
                        if(a.type == VAL_DEC && a.data.d > b.data.d) swap = 1;
                        if(a.type == VAL_TEXTO && strcmp(a.data.s, b.data.s) > 0) swap = 1;
                        if(swap) { lista.data.l.itens[j] = b; lista.data.l.itens[j+1] = a; }
                    }
                }
            }
            return val;
        }
    }
    if (node->type == AST_BINARIO) {
        LebreValue l = avalia_expressao(node->left, env), r = avalia_expressao(node->right, env);
        if (node->op == T_SOMA) {
            if (l.type == VAL_TEXTO || r.type == VAL_TEXTO) {
                char bL[256], bR[256];
                if (l.type == VAL_TEXTO) strcpy(bL, l.data.s); else if (l.type == VAL_INT) sprintf(bL, "%d", l.data.i); else sprintf(bL, "%f", l.data.d);
                if (r.type == VAL_TEXTO) strcpy(bR, r.data.s); else if (r.type == VAL_INT) sprintf(bR, "%d", r.data.i); else sprintf(bR, "%f", r.data.d);
                char* concat = malloc(strlen(bL) + strlen(bR) + 1); strcpy(concat, bL); strcat(concat, bR);
                val.type = VAL_TEXTO; val.data.s = concat; return val;
            }
            if (l.type == VAL_DEC || r.type == VAL_DEC) { val.type = VAL_DEC; val.data.d = (l.type==VAL_DEC?l.data.d:l.data.i) + (r.type==VAL_DEC?r.data.d:r.data.i); return val; }
            val.data.i = l.data.i + r.data.i; return val;
        }
    }
    return val;
}

void executa(ASTNode* node, Ambiente* env) {
    if (!node || flag_return) return;
    if (node->type == AST_PROGRAMA || node->type == AST_BLOCO) {
        for (int i = 0; i < node->arg_count; i++) { executa(node->args[i], env); if (flag_return) break; }
    }
    else if (node->type == AST_INCLUIR) {
        FILE* f = fopen(node->name, "rb");
        if (f) {
            fseek(f, 0, SEEK_END); long l = ftell(f); fseek(f, 0, SEEK_SET);
            char* b = malloc(l + 1); fread(b, 1, l, f); b[l] = '\0'; fclose(f);
            Token* tks = malloc(sizeof(Token) * 10000); lex(b, tks);
            ASTNode* inc_root = parse(tks); executa(inc_root, env);
            free(tks); free(b);
        } else { printf("Erro: Arquivo %s nao encontrado.\n", node->name); }
    }
    else if (node->type == AST_ADICIONAR) {
        LebreValue lista = get_var(env, node->name);
        if (lista.type == VAL_LISTA) {
            lista.data.l.tamanho++;
            lista.data.l.itens = realloc(lista.data.l.itens, sizeof(LebreValue) * lista.data.l.tamanho);
            lista.data.l.itens[lista.data.l.tamanho - 1] = avalia_expressao(node->right, env);
            set_var(env, node->name, lista, 0, node->name[strlen(node->name)-1] == '!');
        }
    }
    else if (node->type == AST_REMOVER) {
        LebreValue lista = get_var(env, node->name);
        LebreValue idx = avalia_expressao(node->right, env);
        if (lista.type == VAL_LISTA && idx.type == VAL_INT && idx.data.i >= 0 && idx.data.i < lista.data.l.tamanho) {
            for(int j = idx.data.i; j < lista.data.l.tamanho - 1; j++) lista.data.l.itens[j] = lista.data.l.itens[j+1];
            lista.data.l.tamanho--;
            if(lista.data.l.tamanho > 0) lista.data.l.itens = realloc(lista.data.l.itens, sizeof(LebreValue) * lista.data.l.tamanho);
            else { free(lista.data.l.itens); lista.data.l.itens = NULL; }
            set_var(env, node->name, lista, 0, node->name[strlen(node->name)-1] == '!');
        }
    }
    else if (node->type == AST_SALVAR_ARQUIVO) {
        LebreValue arq = avalia_expressao(node->left, env); LebreValue txt = avalia_expressao(node->right, env);
        if (arq.type == VAL_TEXTO && txt.type == VAL_TEXTO) { FILE* f = fopen(arq.data.s, "w"); if (f) { fputs(txt.data.s, f); fclose(f); } }
    }
    else if (node->type == AST_RETORNA) { global_return_val = avalia_expressao(node->right, env); flag_return = 1; }
    else if (node->type == AST_FUNCAO_DEF) {
        LebreValue v; v.type = VAL_FUNCAO; v.data.func_def = node; set_var(env, node->name, v, 1, 1);
    }
    else if (node->type == AST_DATA_DEF) {
        LebreValue fmt = avalia_expressao(node->right, env); char res[256]; formatar_data(res, fmt.data.s);
        LebreValue dt; dt.type = VAL_TEXTO; dt.data.s = strdup(res);
        set_var(env, node->name, dt, 0, node->name[strlen(node->name)-1] == '!');
    }
    else if (node->type == AST_DECLARACAO || node->type == AST_CONSTANTE) {
        LebreValue d; d.type = VAL_INT; d.data.i = 0;
        set_var(env, node->name, d, node->type == AST_CONSTANTE, node->name[strlen(node->name)-1] == '!');
    }
    else if (node->type == AST_ATRIBUICAO) {
        set_var(env, node->name, avalia_expressao(node->right, env), 0, node->name[strlen(node->name)-1] == '!');
    }
    else if (node->type == AST_INDEX_ATRIBUICAO) {
        LebreValue lista = get_var(env, node->name); LebreValue idx = avalia_expressao(node->left, env);
        if (lista.type == VAL_LISTA && idx.type == VAL_INT && idx.data.i >= 0 && idx.data.i < lista.data.l.tamanho) {
            lista.data.l.itens[idx.data.i] = avalia_expressao(node->right, env);
        }
    }
    else if (node->type == AST_MOSTRAR) {
        LebreValue v = avalia_expressao(node->right, env);
        if (v.type == VAL_INT) printf("%d\n", v.data.i); else if (v.type == VAL_DEC) printf("%f\n", v.data.d); else if (v.type == VAL_TEXTO) printf("%s\n", v.data.s); else if (v.type == VAL_BOOL) printf("%s\n", v.data.b ? "VERDADEIRO" : "FALSO");
    }
}
EOF