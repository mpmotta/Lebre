#include "lebre.h"

int pos = 0; Token* tkns;

ASTNode* create_node(ASTNodeType type) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = type;
    node->op = T_FIM;
    node->var_type = T_FIM;
    node->name[0] = '\0';
    node->left = node->right = node->cond = node->body = node->alt = node->init = node->step = NULL;
    node->args = malloc(sizeof(ASTNode*) * 50);
    node->arg_count = 0;
    return node;
}

int get_precedence(TokenType type) {
    switch(type) {
        case T_OU: return 1;
        case T_E: return 2;
        case T_IDENTICO: case T_IGUAL: case T_DIFERENTE: return 3;
        case T_MAIOR: case T_MENOR: case T_MAIOR_IGUAL: case T_MENOR_IGUAL: return 4;
        case T_SOMA_OP: case T_SUBTRACAO: return 5;
        case T_MULTIPLICACAO: case T_DIVISAO: case T_MODULO: return 6;
        case T_POTENCIA: case T_RAIZ: return 7;
        default: return 0;
    }
}

ASTNode* parse_expression();
ASTNode* parse_statement();
ASTNode* parse_bloco();

ASTNode* parse_primary() {
    ASTNode* node = create_node(AST_LITERAL);
    if (tkns[pos].type == T_CARREGAR) {
        node->type = AST_CHAMADA_NATIVA; node->op = tkns[pos].type; pos++;
        if (tkns[pos].type == T_ABRE_PAR) pos++;
        if (tkns[pos].type == T_FECHA_PAR) pos++;
        return node;
    }
    if (tkns[pos].type == T_PARA_INT || tkns[pos].type == T_PARA_DECIMAL || tkns[pos].type == T_PARA_TEXTO || tkns[pos].type == T_ORDENAR || tkns[pos].type == T_LER_ARQUIVO || tkns[pos].type == T_ALEATORIO || tkns[pos].type == T_DIVIDIR || tkns[pos].type == T_MEDIA || tkns[pos].type == T_SOMA || tkns[pos].type == T_MAXIMO || tkns[pos].type == T_MINIMO || tkns[pos].type == T_CONTAR) {
        node->type = AST_CHAMADA_NATIVA; node->op = tkns[pos].type; pos++;
        if (tkns[pos].type == T_ABRE_PAR) pos++;
        while (tkns[pos].type != T_FECHA_PAR && tkns[pos].type != T_FIM) {
            if (tkns[pos].type == T_VIRGULA) pos++;
            else node->args[node->arg_count++] = parse_expression();
        }
        if (tkns[pos].type == T_FECHA_PAR) pos++;
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
            while (tkns[pos].type != T_FECHA_PAR && tkns[pos].type != T_FIM) {
                if (tkns[pos].type == T_VIRGULA) pos++;
                else node->args[node->arg_count++] = parse_expression();
            }
            if (tkns[pos].type == T_FECHA_PAR) pos++;
            return node;
        }
        node->type = AST_VARIAVEL;
        return node;
    }
    if (tkns[pos].type == T_ABRE_PAR) {
        node = create_node(AST_LITERAL);
        pos++;
        while (tkns[pos].type != T_FECHA_PAR && tkns[pos].type != T_FIM) {
            if (tkns[pos].type == T_VIRGULA) {
                pos++;
            } else {
                node->args[node->arg_count++] = parse_expression();
            }
        }
        if (tkns[pos].type == T_FECHA_PAR) pos++;
        return node;
    }
    
    char msg[300];
    if (tkns[pos].type == T_FIM) {
        sprintf(msg, "Expressao incompleta");
    } else {
        sprintf(msg, "Expressao invalida perto de '%s'", tkns[pos].value);
    }
    lancar_erro(msg);
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
        bloco->args[bloco->arg_count++] = parse_statement(); 
    }
    if (tkns[pos].type == T_FECHA_CHAVE) pos++;
    return bloco;
}

ASTNode* parse_statement() {
    ASTNode* node = NULL;

    if (tkns[pos].type == T_PONTO_VIRGULA) {
        pos++;
        return create_node(AST_VAZIO);
    }
    
    if (tkns[pos].type == T_SE) {
        node = create_node(AST_SE); pos++;
        if (tkns[pos].type == T_ABRE_PAR) pos++;
        node->cond = parse_expression();
        if (tkns[pos].type == T_FECHA_PAR) pos++;
        node->body = parse_bloco();
        
        ASTNode* curr_alt = node;
        while (tkns[pos].type == T_SENAO) {
            pos++;
            if (tkns[pos].type == T_SE) {
                pos++;
                ASTNode* senao_se = create_node(AST_SE);
                if (tkns[pos].type == T_ABRE_PAR) pos++;
                senao_se->cond = parse_expression();
                if (tkns[pos].type == T_FECHA_PAR) pos++;
                senao_se->body = parse_bloco();
                curr_alt->alt = senao_se;
                curr_alt = senao_se;
            } else {
                curr_alt->alt = parse_bloco();
                break;
            }
        }
        return node;
    }
    
    if (tkns[pos].type == T_ENQUANTO) {
        node = create_node(AST_ENQUANTO); pos++;
        if (tkns[pos].type == T_ABRE_PAR) pos++;
        node->cond = parse_expression();
        if (tkns[pos].type == T_FECHA_PAR) pos++;
        node->body = parse_bloco();
        return node;
    }

    if (tkns[pos].type == T_LINHA) {
        node = create_node(AST_LINHA); pos++;
        if (tkns[pos].type == T_ABRE_PAR) pos++;
        node->init = parse_statement();
        node->cond = parse_expression();
        if (tkns[pos].type == T_PONTO_VIRGULA) pos++;
        node->step = parse_statement();
        if (tkns[pos].type == T_FECHA_PAR) pos++;
        node->body = parse_bloco();
        return node;
    }

    if (tkns[pos].type == T_LINHAS) {
        node = create_node(AST_LINHAS); pos++;
        if (tkns[pos].type == T_ABRE_PAR) pos++;
        if (tkns[pos].type == T_VAR) pos++;
        if (tkns[pos].type == T_INT || tkns[pos].type == T_TEXTO || tkns[pos].type == T_DECIMAL || tkns[pos].type == T_BOOLEANO) pos++;
        strcpy(node->name, tkns[pos].value); pos++; 
        if (tkns[pos].type == T_DOIS_PONTOS) pos++;
        node->right = parse_expression();
        if (tkns[pos].type == T_FECHA_PAR) pos++;
        node->body = parse_bloco();
        return node;
    }

    if (tkns[pos].type == T_INCLUIR) {
        node = create_node(AST_INCLUIR); pos++;
        if (tkns[pos].type == T_ABRE_PAR) pos++;
        if (tkns[pos].type == T_STRING) { strcpy(node->name, tkns[pos].value); pos++; }
        if (tkns[pos].type == T_FECHA_PAR) pos++;
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
        node = create_node(AST_FUNCAO_DEF); pos++;
        if (tkns[pos].type == T_INT || tkns[pos].type == T_TEXTO || tkns[pos].type == T_DECIMAL || tkns[pos].type == T_BOOLEANO || tkns[pos].type == T_LISTA) pos++;
        strcpy(node->name, tkns[pos].value); pos++;
        if (tkns[pos].type == T_ABRE_PAR) pos++;
        while (tkns[pos].type != T_FECHA_PAR && tkns[pos].type != T_FIM) {
            if (tkns[pos].type == T_VIRGULA || tkns[pos].type == T_VAR || tkns[pos].type == T_INT || tkns[pos].type == T_TEXTO || tkns[pos].type == T_DECIMAL || tkns[pos].type == T_BOOLEANO || tkns[pos].type == T_LISTA) pos++;
            else { ASTNode* arg = create_node(AST_VARIAVEL); strcpy(arg->name, tkns[pos].value); node->args[node->arg_count++] = arg; pos++; }
        }
        if (tkns[pos].type == T_FECHA_PAR) pos++;
        node->body = parse_bloco();
        return node;
    }
    if (tkns[pos].type == T_VAR || tkns[pos].type == T_CONS) {
        int is_const = (tkns[pos].type == T_CONS); pos++;
        TokenType v_type = T_INT;
        if (tkns[pos].type == T_INT || tkns[pos].type == T_TEXTO || tkns[pos].type == T_DECIMAL || tkns[pos].type == T_BOOLEANO || tkns[pos].type == T_LISTA) {
            v_type = tkns[pos].type;
            pos++;
        }
        if (tkns[pos].type == T_DATA) {
            node = create_node(AST_DATA_DEF); pos++;
            strcpy(node->name, tkns[pos].value); pos++;
            if (tkns[pos].type == T_ATRIBUICAO) pos++;
            if (tkns[pos].type == T_ABRE_PAR) pos++;
            node->right = parse_expression();
            if (tkns[pos].type == T_FECHA_PAR) pos++;
            if (tkns[pos].type == T_PONTO_VIRGULA) pos++;
            return node;
        }
        if (tkns[pos].type != T_IDENTIFICADOR && tkns[pos].type != T_SCREAM_SNAKE) {
            lancar_erro("Nome de variavel ou constante invalido na declaracao");
        }
        node = create_node(is_const ? AST_CONSTANTE : AST_DECLARACAO);
        node->var_type = v_type;
        strcpy(node->name, tkns[pos].value); pos++;
        
        if (tkns[pos].type == T_ATRIBUICAO) {
            pos++;
            node->right = parse_expression();
        }
        
        if (tkns[pos].type == T_PONTO_VIRGULA) pos++;
        return node;
    }

    if (tkns[pos].type == T_PARA_INT || tkns[pos].type == T_PARA_DECIMAL || tkns[pos].type == T_PARA_TEXTO || 
        tkns[pos].type == T_ORDENAR || tkns[pos].type == T_LER_ARQUIVO || tkns[pos].type == T_ALEATORIO || 
        tkns[pos].type == T_DIVIDIR || tkns[pos].type == T_MEDIA || tkns[pos].type == T_SOMA || 
        tkns[pos].type == T_MAXIMO || tkns[pos].type == T_MINIMO || tkns[pos].type == T_CONTAR || 
        tkns[pos].type == T_CARREGAR) {
        
        ASTNode* expr = parse_expression();
        if (tkns[pos].type == T_PONTO_VIRGULA) pos++;
        return expr;
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
            pos--; 
            ASTNode* expr = parse_expression();
            if (tkns[pos].type == T_PONTO_VIRGULA) pos++;
            return expr;
        }
        
        char msg[300];
        sprintf(msg, "Comando ou instrucao invalida: '%s'", temp_name);
        lancar_erro(msg);
    }
    
    if (tkns[pos].type == T_MOSTRAR) {
        node = create_node(AST_MOSTRAR); pos++;
        if (tkns[pos].type == T_ABRE_PAR) pos++;
        node->right = parse_expression();
        if (tkns[pos].type == T_FECHA_PAR) pos++;
        if (tkns[pos].type == T_PONTO_VIRGULA) pos++;
        return node;
    }
    
    char msg[300];
    sprintf(msg, "Sintaxe invalida perto de '%s'", tkns[pos].value);
    lancar_erro(msg);
    return create_node(AST_VAZIO);
}

ASTNode* parse(Token* tokens) {
    tkns = tokens; pos = 0;
    ASTNode* root = create_node(AST_PROGRAMA);
    while (tkns[pos].type != T_FIM) {
        root->args[root->arg_count++] = parse_statement();
    }
    return root;
}