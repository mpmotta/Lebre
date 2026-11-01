#include "lebre.h"

int pos = 0; Token* tkns;

ASTNode* create_node(ASTNodeType type) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = type;
    node->op = T_FIM;
    node->var_type = T_FIM;
    node->visibilidade = T_PUBLICO;
    node->is_abstract = 0;
    
    if (tkns) {
        node->numero_linha = tkns[pos].numero_linha;
        strcpy(node->arquivo, tkns[pos].arquivo);
    } else {
        node->numero_linha = 1;
        strcpy(node->arquivo, "desconhecido");
    }

    node->name[0] = '\0';
    node->prop_name[0] = '\0';
    node->parent_name[0] = '\0';
    node->left = node->right = node->cond = node->body = node->alt = node->init = node->step = NULL;
    node->args = malloc(sizeof(ASTNode*) * 50);
    node->arg_count = 0;
    return node;
}

void exigir_ponto_virgula() {
    if (tkns[pos].type == T_PONTO_VIRGULA) {
        pos++;
    } else {
        char msg[256];
        sprintf(msg, "Faltou ponto e virgula no final da instrucao perto de '%s' (Arquivo: %s, Linha: %d)", tkns[pos].value, tkns[pos].arquivo, tkns[pos].numero_linha);
        lancar_erro(msg);
    }
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

ASTNode* parse_string_interp(const char* str) {
    char buffer[256] = {0};
    int i = 0, buf_i = 0;
    ASTNode* root = NULL;

    while (str[i] != '\0') {
        if (str[i] == '$' && str[i+1] == '{') {
            if (buf_i > 0) {
                buffer[buf_i] = '\0';
                ASTNode* lit = create_node(AST_LITERAL);
                lit->op = T_STRING;
                strcpy(lit->name, buffer);
                if (!root) {
                    root = lit;
                } else {
                    ASTNode* bin = create_node(AST_BINARIO);
                    bin->op = T_SOMA_OP;
                    bin->left = root;
                    bin->right = lit;
                    root = bin;
                }
                buf_i = 0;
            }
            i += 2;
            char var_name[128] = {0};
            int v_i = 0;
            while (str[i] != '\0' && str[i] != '}') {
                var_name[v_i++] = str[i++];
            }
            if (str[i] == '}') i++;
            
            ASTNode* var_node = create_node(AST_VARIAVEL);
            strcpy(var_node->name, var_name);
            
            if (!root) {
                root = var_node;
            } else {
                ASTNode* bin = create_node(AST_BINARIO);
                bin->op = T_SOMA_OP;
                bin->left = root;
                bin->right = var_node;
                root = bin;
            }
        } else {
            buffer[buf_i++] = str[i++];
        }
    }
    
    if (buf_i > 0) {
        buffer[buf_i] = '\0';
        ASTNode* lit = create_node(AST_LITERAL);
        lit->op = T_STRING;
        strcpy(lit->name, buffer);
        if (!root) {
            root = lit;
        } else {
            ASTNode* bin = create_node(AST_BINARIO);
            bin->op = T_SOMA_OP;
            bin->left = root;
            bin->right = lit;
            root = bin;
        }
    }
    
    if (!root) {
        root = create_node(AST_LITERAL);
        root->op = T_STRING;
        strcpy(root->name, "");
    }
    return root;
}

ASTNode* parse_primary() {
    ASTNode* node = create_node(AST_LITERAL);
    if (tkns[pos].type == T_CARREGAR) {
        node->type = AST_CHAMADA_NATIVA; node->op = tkns[pos].type; pos++;
        if (tkns[pos].type == T_ABRE_PAR) pos++;
        if (tkns[pos].type == T_FECHA_PAR) pos++;
        return node;
    }
    if (tkns[pos].type == T_NOVO) {
        node->type = AST_INSTANCIA; pos++;
        strcpy(node->name, tkns[pos].value); pos++;
        if (tkns[pos].type == T_ABRE_PAR) pos++;
        while (tkns[pos].type != T_FECHA_PAR && tkns[pos].type != T_FIM) {
            if (tkns[pos].type == T_VIRGULA) pos++;
            else node->args[node->arg_count++] = parse_expression();
        }
        if (tkns[pos].type == T_FECHA_PAR) pos++;
        return node;
    }
    if (tkns[pos].type == T_PARA_INT || tkns[pos].type == T_PARA_DECIMAL || tkns[pos].type == T_PARA_TEXTO || tkns[pos].type == T_ORDENAR || tkns[pos].type == T_LER_ARQUIVO || tkns[pos].type == T_ALEATORIO || tkns[pos].type == T_DIVIDIR || tkns[pos].type == T_MEDIA || tkns[pos].type == T_SOMA || tkns[pos].type == T_MAXIMO || tkns[pos].type == T_MINIMO || tkns[pos].type == T_CONTAR || tkns[pos].type == T_ARRED || tkns[pos].type == T_TETO || tkns[pos].type == T_PISO) {
        node->type = AST_CHAMADA_NATIVA; node->op = tkns[pos].type; pos++;
        if (tkns[pos].type == T_ABRE_PAR) pos++;
        while (tkns[pos].type != T_FECHA_PAR && tkns[pos].type != T_FIM) {
            if (tkns[pos].type == T_VIRGULA) pos++;
            else node->args[node->arg_count++] = parse_expression();
        }
        if (tkns[pos].type == T_FECHA_PAR) pos++;
        return node;
    }
    if (tkns[pos].type == T_STRING) {
        ASTNode* interp = parse_string_interp(tkns[pos].value);
        pos++;
        free(node);
        return interp;
    }
    if (tkns[pos].type == T_NUM_INT || tkns[pos].type == T_NUM_DEC || tkns[pos].type == T_VERDADEIRO || tkns[pos].type == T_FALSO) {
        strcpy(node->name, tkns[pos].value); node->op = tkns[pos].type; pos++;
        return node;
    }
    if (tkns[pos].type == T_IDENTIFICADOR || tkns[pos].type == T_SCREAM_SNAKE) {
        strcpy(node->name, tkns[pos].value); pos++;
        if (tkns[pos].type == T_SETA) {
            pos++;
            char tmp_name[128];
            strcpy(tmp_name, tkns[pos].value); pos++;
            if (tkns[pos].type == T_ABRE_PAR) {
                node->type = AST_METODO_CHAMADA; pos++;
                strcpy(node->prop_name, tmp_name);
                while (tkns[pos].type != T_FECHA_PAR && tkns[pos].type != T_FIM) {
                    if (tkns[pos].type == T_VIRGULA) pos++;
                    else node->args[node->arg_count++] = parse_expression();
                }
                if (tkns[pos].type == T_FECHA_PAR) pos++;
                return node;
            }
            node->type = AST_PROPRIEDADE_ACESSO;
            if (tmp_name[0] != '#') {
                sprintf(node->prop_name, "#%s", tmp_name);
            } else {
                strcpy(node->prop_name, tmp_name);
            }
            return node;
        }
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
        free(node);
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
        sprintf(msg, "Expressao incompleta (Arquivo: %s, Linha: %d)", tkns[pos > 0 ? pos-1 : pos].arquivo, tkns[pos > 0 ? pos-1 : pos].numero_linha);
    } else {
        sprintf(msg, "Expressao invalida perto de '%s' (Arquivo: %s, Linha: %d)", tkns[pos].value, tkns[pos].arquivo, tkns[pos].numero_linha);
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

ASTNode* parse_expression() { 
    ASTNode* expr = parse_binop(parse_primary(), 1);
    if (tkns[pos].type == T_INTERROGACAO) {
        pos++;
        ASTNode* node_ternario = create_node(AST_TERNARIO);
        node_ternario->cond = expr;
        node_ternario->left = parse_expression();
        if (tkns[pos].type == T_DOIS_PONTOS) {
            pos++;
        } else {
            char msg[256];
            sprintf(msg, "Faltou ':' no operador ternario (Arquivo: %s, Linha: %d)", tkns[pos].arquivo, tkns[pos].numero_linha);
            lancar_erro(msg);
        }
        node_ternario->right = parse_expression();
        return node_ternario;
    }
    return expr;
}

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
    TokenType visibilidade_atual = T_PUBLICO;

    if (tkns[pos].type == T_PONTO_VIRGULA) {
        pos++;
        return create_node(AST_VAZIO);
    }
    
    if (tkns[pos].type == T_PUBLICO || tkns[pos].type == T_PRIVADO || tkns[pos].type == T_PROTEGIDO) {
        visibilidade_atual = tkns[pos].type;
        pos++;
    }
    
    if (tkns[pos].type == T_CLASSE) {
        node = create_node(AST_CLASSE_DEF); 
        pos++;
        
        int is_abstract = 0;
        if (tkns[pos].type == T_ABSTRATA) {
            is_abstract = 1;
            pos++;
        }
        
        node->is_abstract = is_abstract;
        strcpy(node->name, tkns[pos].value); 
        pos++;
        
        strcpy(node->parent_name, "");
        if (tkns[pos].type == T_ESTENDE) {
            pos++;
            strcpy(node->parent_name, tkns[pos].value);
            pos++;
        }
        node->body = parse_bloco();
        return node;
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
        exigir_ponto_virgula();
        node->step = parse_statement();
        if (tkns[pos].type == T_FECHA_PAR) pos++;
        node->body = parse_bloco();
        return node;
    }

    if (tkns[pos].type == T_LINHAS) {
        node = create_node(AST_LINHAS); pos++;
        if (tkns[pos].type == T_ABRE_PAR) pos++;
        if (tkns[pos].type == T_VAR) pos++;
        if (tkns[pos].type == T_INT || tkns[pos].type == T_TEXTO || tkns[pos].type == T_DECIMAL || tkns[pos].type == T_BOOLEANO || tkns[pos].type == T_OBJETO) pos++;
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
        exigir_ponto_virgula();
        return node;
    }
    if (tkns[pos].type == T_ADICIONAR) {
        node = create_node(AST_ADICIONAR); pos++;
        if (tkns[pos].type == T_ABRE_PAR) pos++;
        strcpy(node->name, tkns[pos].value); pos++; 
        if (tkns[pos].type == T_VIRGULA) pos++;
        node->right = parse_expression();
        if (tkns[pos].type == T_FECHA_PAR) pos++;
        exigir_ponto_virgula();
        return node;
    }
    if (tkns[pos].type == T_REMOVER) {
        node = create_node(AST_REMOVER); pos++;
        if (tkns[pos].type == T_ABRE_PAR) pos++;
        strcpy(node->name, tkns[pos].value); pos++; 
        if (tkns[pos].type == T_VIRGULA) pos++;
        node->right = parse_expression();
        if (tkns[pos].type == T_FECHA_PAR) pos++;
        exigir_ponto_virgula();
        return node;
    }
    if (tkns[pos].type == T_SALVAR_ARQUIVO) {
        node = create_node(AST_SALVAR_ARQUIVO); pos++;
        if (tkns[pos].type == T_ABRE_PAR) pos++;
        node->left = parse_expression(); 
        if (tkns[pos].type == T_VIRGULA) pos++;
        node->right = parse_expression(); 
        if (tkns[pos].type == T_FECHA_PAR) pos++;
        exigir_ponto_virgula();
        return node;
    }
    if (tkns[pos].type == T_RETORNA) {
        node = create_node(AST_RETORNA); pos++;
        if (tkns[pos].type == T_PONTO_VIRGULA) {
            node->right = NULL;
        } else {
            node->right = parse_expression();
        }
        exigir_ponto_virgula();
        return node;
    }
    if (tkns[pos].type == T_FUNCAO) {
        node = create_node(AST_FUNCAO_DEF); pos++;
        node->visibilidade = visibilidade_atual;
        if (tkns[pos].type == T_INT || tkns[pos].type == T_TEXTO || tkns[pos].type == T_DECIMAL || tkns[pos].type == T_BOOLEANO || tkns[pos].type == T_LISTA || tkns[pos].type == T_OBJETO) pos++;
        strcpy(node->name, tkns[pos].value); pos++;
        if (tkns[pos].type == T_ABRE_PAR) pos++;
        
        while (tkns[pos].type != T_FECHA_PAR && tkns[pos].type != T_FIM) {
            if (tkns[pos].type == T_VIRGULA) {
                pos++;
            } else if (tkns[pos].type == T_VAR || tkns[pos].type == T_INT || tkns[pos].type == T_TEXTO || tkns[pos].type == T_DECIMAL || tkns[pos].type == T_BOOLEANO || tkns[pos].type == T_LISTA || tkns[pos].type == T_OBJETO) {
                pos++;
            } else if (tkns[pos].type == T_IDENTIFICADOR || tkns[pos].type == T_SCREAM_SNAKE) {
                ASTNode* arg = create_node(AST_VARIAVEL); 
                strcpy(arg->name, tkns[pos].value); 
                node->args[node->arg_count++] = arg; 
                pos++;
            } else {
                pos++;
            }
        }
        if (tkns[pos].type == T_FECHA_PAR) pos++;
        node->body = parse_bloco();
        return node;
    }
    if (tkns[pos].type == T_VAR || tkns[pos].type == T_CONS || tkns[pos].type == T_OBJETO) {
        int is_const = (tkns[pos].type == T_CONS);
        int is_obj = (tkns[pos].type == T_OBJETO);
        pos++;
        TokenType v_type = T_INT;
        if (is_obj) {
            v_type = T_OBJETO;
        } else if (tkns[pos].type == T_INT || tkns[pos].type == T_TEXTO || tkns[pos].type == T_DECIMAL || tkns[pos].type == T_BOOLEANO || tkns[pos].type == T_LISTA) {
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
            exigir_ponto_virgula();
            return node;
        }
        if (tkns[pos].type != T_IDENTIFICADOR && tkns[pos].type != T_SCREAM_SNAKE) {
            char msg[256];
            sprintf(msg, "Nome de variavel ou constante invalido na declaracao (Arquivo: %s, Linha: %d)", tkns[pos].arquivo, tkns[pos].numero_linha);
            lancar_erro(msg);
        }
        node = create_node(is_const ? AST_CONSTANTE : AST_DECLARACAO);
        node->var_type = v_type;
        node->visibilidade = visibilidade_atual;
        strcpy(node->name, tkns[pos].value); pos++;
        
        if (tkns[pos].type == T_ATRIBUICAO) {
            pos++;
            node->right = parse_expression();
        }
        
        exigir_ponto_virgula();
        return node;
    }

    if (tkns[pos].type == T_PARA_INT || tkns[pos].type == T_PARA_DECIMAL || tkns[pos].type == T_PARA_TEXTO || 
        tkns[pos].type == T_ORDENAR || tkns[pos].type == T_LER_ARQUIVO || tkns[pos].type == T_ALEATORIO || 
        tkns[pos].type == T_DIVIDIR || tkns[pos].type == T_MEDIA || tkns[pos].type == T_SOMA || 
        tkns[pos].type == T_MAXIMO || tkns[pos].type == T_MINIMO || tkns[pos].type == T_CONTAR || 
        tkns[pos].type == T_ARRED || tkns[pos].type == T_TETO || tkns[pos].type == T_PISO || 
        tkns[pos].type == T_CARREGAR) {
        
        ASTNode* expr = parse_expression();
        exigir_ponto_virgula();
        return expr;
    }

    if (tkns[pos].type == T_IDENTIFICADOR || tkns[pos].type == T_SCREAM_SNAKE) {
        char temp_name[128]; strcpy(temp_name, tkns[pos].value); pos++;
        if (tkns[pos].type == T_SETA) {
            pos++;
            char prop_name[128]; strcpy(prop_name, tkns[pos].value); pos++;
            if (tkns[pos].type == T_ATRIBUICAO) {
                node = create_node(AST_PROPRIEDADE_ATRIBUICAO);
                strcpy(node->name, temp_name);
                if (prop_name[0] != '#') {
                    sprintf(node->prop_name, "#%s", prop_name);
                } else {
                    strcpy(node->prop_name, prop_name);
                }
                pos++;
                node->right = parse_expression();
                exigir_ponto_virgula();
                return node;
            }
            if (tkns[pos].type == T_ABRE_PAR) {
                pos -= 3;
                ASTNode* expr = parse_expression();
                exigir_ponto_virgula();
                return expr;
            }
        }
        if (tkns[pos].type == T_ABRE_COL) {
            node = create_node(AST_INDEX_ATRIBUICAO); strcpy(node->name, temp_name); pos++;
            node->left = parse_expression();
            if (tkns[pos].type == T_FECHA_COL) pos++;
            if (tkns[pos].type == T_ATRIBUICAO) { pos++; node->right = parse_expression(); }
            exigir_ponto_virgula();
            return node;
        }
        if (tkns[pos].type == T_INCREMENTO || tkns[pos].type == T_DECREMENTO) {
            node = create_node(tkns[pos].type == T_INCREMENTO ? AST_INCREMENTO : AST_DECREMENTO);
            strcpy(node->name, temp_name); pos++;
            exigir_ponto_virgula();
            return node;
        }
        if (tkns[pos].type == T_ATRIBUICAO) {
            node = create_node(AST_ATRIBUICAO); strcpy(node->name, temp_name); pos++;
            node->right = parse_expression();
            exigir_ponto_virgula();
            return node;
        }
        if (tkns[pos].type == T_ABRE_PAR) {
            pos--; 
            ASTNode* expr = parse_expression();
            exigir_ponto_virgula();
            return expr;
        }
        
        char msg[300];
        sprintf(msg, "Comando ou instrucao invalida: '%s' (Arquivo: %s, Linha: %d)", temp_name, tkns[pos].arquivo, tkns[pos].numero_linha);
        lancar_erro(msg);
    }
    
    if (tkns[pos].type == T_MOSTRAR) {
        node = create_node(AST_MOSTRAR); pos++;
        if (tkns[pos].type == T_ABRE_PAR) pos++;
        node->right = parse_expression();
        if (tkns[pos].type == T_FECHA_PAR) pos++;
        exigir_ponto_virgula();
        return node;
    }
    
    char msg[300];
    sprintf(msg, "Sintaxe invalida perto de '%s' (Arquivo: %s, Linha: %d)", tkns[pos].value, tkns[pos].arquivo, tkns[pos].numero_linha);
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