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

double texto_para_decimal(const char* s) {
    char copia[128];
    strncpy(copia, s, 127);
    copia[127] = '\0';
    for (int i = 0; copia[i] != '\0'; i++) {
        if (copia[i] == ',') copia[i] = '.';
    }
    return atof(copia);
}

LebreValue avalia_expressao(ASTNode* node, Ambiente* env) {
    LebreValue val; val.type = VAL_INT; val.data.i = 0;
    if (!node) return val;
    if (node->type == AST_LITERAL) {
        if (node->op == T_NUM_INT) { val.type = VAL_INT; val.data.i = atoi(node->name); }
        else if (node->op == T_NUM_DEC) { val.type = VAL_DEC; val.data.d = texto_para_decimal(node->name); }
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
        lancar_erro("Indice de lista fora dos limites");
        return val;
    }
    if (node->type == AST_CHAMADA_USER_FUNC) {
        LebreValue func = get_var(env, node->name);
        
        if (func.type == VAL_FUNCAO_C) {
            return func.data.func_c(env, node->args, node->arg_count);
        }
        
        if (func.type != VAL_FUNCAO) {
            lancar_erro("Tentativa de chamar algo que nao e uma funcao");
        }
        Ambiente* func_env = malloc(sizeof(Ambiente));
        func_env->count = 0; func_env->pai = &ambiente_global;
        for(int i=0; i < node->arg_count; i++) {
            set_var(func_env, func.data.func_def->args[i]->name, avalia_expressao(node->args[i], env), 0, 0);
        }
        int old_flag = flag_return;
        flag_return = 0;
        executa(func.data.func_def->body, func_env);
        LebreValue res = global_return_val;
        flag_return = old_flag;
        free(func_env);
        return res;
    }
    if (node->type == AST_CHAMADA_NATIVA) {
        if (node->op == T_CARREGAR) {
            char buf[1024];
            if (fgets(buf, sizeof(buf), stdin)) buf[strcspn(buf, "\r\n")] = '\0';
            else buf[0] = '\0';
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
            if (node->op == T_PARA_DECIMAL) { val.type = VAL_DEC; val.data.d = (a.type == VAL_TEXTO) ? texto_para_decimal(a.data.s) : (a.type == VAL_INT ? (double)a.data.i : a.data.d); }
            if (node->op == T_PARA_TEXTO) {
                char b[64]; if (a.type == VAL_INT) sprintf(b, "%d", a.data.i); else if (a.type == VAL_DEC) sprintf(b, "%g", a.data.d); else strcpy(b, a.data.s);
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
        if (node->op == T_MEDIA || node->op == T_SOMA || node->op == T_MAXIMO || node->op == T_MINIMO) {
            double calc_dec = 0; int is_dec = 0; int total_itens = 0;
            
            if (node->arg_count == 1) {
                LebreValue unico = avalia_expressao(node->args[0], env);
                if (unico.type == VAL_LISTA) {
                    total_itens = unico.data.l.tamanho;
                    for (int i = 0; i < total_itens; i++) {
                        LebreValue item = unico.data.l.itens[i];
                        double val_d = (item.type == VAL_DEC) ? item.data.d : (item.type == VAL_TEXTO ? texto_para_decimal(item.data.s) : (double)item.data.i);
                        if (item.type == VAL_DEC) is_dec = 1;
                        if (i == 0 && (node->op == T_MAXIMO || node->op == T_MINIMO)) calc_dec = val_d;
                        else {
                            if (node->op == T_MEDIA || node->op == T_SOMA) calc_dec += val_d;
                            if (node->op == T_MAXIMO && val_d > calc_dec) calc_dec = val_d;
                            if (node->op == T_MINIMO && val_d < calc_dec) calc_dec = val_d;
                        }
                    }
                } else {
                    total_itens = 1;
                    calc_dec = (unico.type == VAL_DEC) ? unico.data.d : (double)unico.data.i;
                    if (unico.type == VAL_DEC) is_dec = 1;
                }
            } else {
                total_itens = node->arg_count;
                for (int i = 0; i < node->arg_count; i++) {
                    LebreValue arg = avalia_expressao(node->args[i], env);
                    double val_d = arg.type == VAL_DEC ? arg.data.d : (arg.type == VAL_TEXTO ? texto_para_decimal(arg.data.s) : (double)arg.data.i);
                    if (arg.type == VAL_DEC) is_dec = 1;
                    
                    if (i == 0 && (node->op == T_MAXIMO || node->op == T_MINIMO)) {
                        calc_dec = val_d;
                    } else {
                        if (node->op == T_MEDIA || node->op == T_SOMA) calc_dec += val_d;
                        if (node->op == T_MAXIMO && val_d > calc_dec) calc_dec = val_d;
                        if (node->op == T_MINIMO && val_d < calc_dec) calc_dec = val_d;
                    }
                }
            }

            if (node->op == T_MEDIA) {
                val.type = VAL_DEC; val.data.d = total_itens > 0 ? calc_dec / total_itens : 0;
                return val;
            }
            if (is_dec) { val.type = VAL_DEC; val.data.d = calc_dec; }
            else { val.type = VAL_INT; val.data.i = (int)calc_dec; }
            return val;
        }
        if (node->op == T_CONTAR) {
            val.type = VAL_INT; val.data.i = node->arg_count;
            if (node->arg_count == 1) {
                LebreValue arg = avalia_expressao(node->args[0], env);
                if (arg.type == VAL_LISTA) val.data.i = arg.data.l.tamanho;
                else if (arg.type == VAL_TEXTO) val.data.i = strlen(arg.data.s);
            }
            return val;
        }
    }
    if (node->type == AST_BINARIO) {
        LebreValue l = avalia_expressao(node->left, env);
        LebreValue r = avalia_expressao(node->right, env);
        
        if (node->op == T_SOMA_OP) {
            if (l.type == VAL_TEXTO || r.type == VAL_TEXTO) {
                char bL[512], bR[512];
                if (l.type == VAL_TEXTO) strcpy(bL, l.data.s); else if (l.type == VAL_INT) sprintf(bL, "%d", l.data.i); else if (l.type == VAL_DEC) sprintf(bL, "%g", l.data.d); else sprintf(bL, "%s", l.data.b ? "VERDADEIRO" : "FALSO");
                if (r.type == VAL_TEXTO) strcpy(bR, r.data.s); else if (r.type == VAL_INT) sprintf(bR, "%d", r.data.i); else if (r.type == VAL_DEC) sprintf(bR, "%g", r.data.d); else sprintf(bR, "%s", r.data.b ? "VERDADEIRO" : "FALSO");
                char* concat = malloc(strlen(bL) + strlen(bR) + 1); strcpy(concat, bL); strcat(concat, bR);
                val.type = VAL_TEXTO; val.data.s = concat; return val;
            }
            if (l.type == VAL_DEC || r.type == VAL_DEC) { val.type = VAL_DEC; val.data.d = (l.type==VAL_DEC?l.data.d:l.data.i) + (r.type==VAL_DEC?r.data.d:r.data.i); return val; }
            val.type = VAL_INT; val.data.i = l.data.i + r.data.i; return val;
        }
        else if (node->op == T_SUBTRACAO) {
            if (l.type == VAL_DEC || r.type == VAL_DEC) { val.type = VAL_DEC; val.data.d = (l.type==VAL_DEC?l.data.d:l.data.i) - (r.type==VAL_DEC?r.data.d:r.data.i); return val; }
            val.type = VAL_INT; val.data.i = l.data.i - r.data.i; return val;
        }
        else if (node->op == T_MULTIPLICACAO) {
            if (l.type == VAL_DEC || r.type == VAL_DEC) { val.type = VAL_DEC; val.data.d = (l.type==VAL_DEC?l.data.d:l.data.i) * (r.type==VAL_DEC?r.data.d:r.data.i); return val; }
            val.type = VAL_INT; val.data.i = l.data.i * r.data.i; return val;
        }
        else if (node->op == T_DIVISAO) {
            double divisor = (r.type==VAL_DEC?r.data.d:r.data.i);
            if (divisor == 0) { lancar_erro("Divisao por zero"); }
            if (l.type == VAL_DEC || r.type == VAL_DEC) {
                val.type = VAL_DEC; val.data.d = (l.type==VAL_DEC?l.data.d:l.data.i) / divisor;
                return val;
            }
            if (l.data.i % r.data.i != 0) {
                val.type = VAL_DEC; val.data.d = (double)l.data.i / (double)r.data.i;
                return val;
            }
            val.type = VAL_INT; val.data.i = l.data.i / r.data.i; return val;
        }
        else if (node->op == T_MODULO) {
            int divisor = (r.type==VAL_DEC?(int)r.data.d:r.data.i);
            if (divisor == 0) { lancar_erro("Divisao por zero"); }
            val.type = VAL_INT; val.data.i = (l.type==VAL_DEC?(int)l.data.d:l.data.i) % divisor; return val;
        }
        else if (node->op == T_POTENCIA) {
            double base = (l.type==VAL_DEC?l.data.d:l.data.i);
            double exp = (r.type==VAL_DEC?r.data.d:r.data.i);
            if (base == 0 && exp < 0) { lancar_erro("Zero elevado a expoente negativo"); }
            if (exp < 0 || l.type == VAL_DEC || r.type == VAL_DEC) {
                val.type = VAL_DEC; val.data.d = pow(base, exp);
                return val;
            }
            val.type = VAL_INT; val.data.i = (int)pow(base, exp); return val;
        }
        else if (node->op == T_RAIZ) {
            double base = (l.type==VAL_DEC?l.data.d:l.data.i);
            double root = (r.type==VAL_DEC?r.data.d:r.data.i);
            if (base < 0) { lancar_erro("Raiz de numero negativo"); }
            if (root == 0) { lancar_erro("Divisao por zero na raiz"); }
            double res = pow(base, 1.0/root);
            if (l.type == VAL_DEC || r.type == VAL_DEC || floor(res) != res) {
                val.type = VAL_DEC; val.data.d = res;
                return val;
            }
            val.type = VAL_INT; val.data.i = (int)res; return val;
        }
        else if (node->op == T_IGUAL || node->op == T_IDENTICO) {
            val.type = VAL_BOOL;
            if (l.type == VAL_TEXTO && r.type == VAL_TEXTO) val.data.b = (strcmp(l.data.s, r.data.s) == 0);
            else val.data.b = ((l.type==VAL_DEC?l.data.d:l.data.i) == (r.type==VAL_DEC?r.data.d:r.data.i));
            return val;
        }
        else if (node->op == T_DIFERENTE) {
            val.type = VAL_BOOL;
            if (l.type == VAL_TEXTO && r.type == VAL_TEXTO) val.data.b = (strcmp(l.data.s, r.data.s) != 0);
            else val.data.b = ((l.type==VAL_DEC?l.data.d:l.data.i) != (r.type==VAL_DEC?r.data.d:r.data.i));
            return val;
        }
        else if (node->op == T_MAIOR) {
            val.type = VAL_BOOL; val.data.b = ((l.type==VAL_DEC?l.data.d:l.data.i) > (r.type==VAL_DEC?r.data.d:r.data.i)); return val;
        }
        else if (node->op == T_MENOR) {
            val.type = VAL_BOOL; val.data.b = ((l.type==VAL_DEC?l.data.d:l.data.i) < (r.type==VAL_DEC?r.data.d:r.data.i)); return val;
        }
        else if (node->op == T_MAIOR_IGUAL) {
            val.type = VAL_BOOL; val.data.b = ((l.type==VAL_DEC?l.data.d:l.data.i) >= (r.type==VAL_DEC?r.data.d:r.data.i)); return val;
        }
        else if (node->op == T_MENOR_IGUAL) {
            val.type = VAL_BOOL; val.data.b = ((l.type==VAL_DEC?l.data.d:l.data.i) <= (r.type==VAL_DEC?r.data.d:r.data.i)); return val;
        }
        else if (node->op == T_E) {
            val.type = VAL_BOOL; val.data.b = ((l.type==VAL_BOOL?l.data.b:l.data.i) && (r.type==VAL_BOOL?r.data.b:r.data.i)); return val;
        }
        else if (node->op == T_OU) {
            val.type = VAL_BOOL; val.data.b = ((l.type==VAL_BOOL?l.data.b:l.data.i) || (r.type==VAL_BOOL?r.data.b:r.data.i)); return val;
        }
    }
    return val;
}

void executa(ASTNode* node, Ambiente* env) {
    if (!node || flag_return) return;
    if (node->type == AST_PROGRAMA || node->type == AST_BLOCO) {
        for (int i = 0; i < node->arg_count; i++) { executa(node->args[i], env); if (flag_return) break; }
    }
    else if (node->type == AST_CHAMADA_NATIVA || node->type == AST_CHAMADA_USER_FUNC) {
        avalia_expressao(node, env);
    }
    else if (node->type == AST_SE) {
        LebreValue cond = avalia_expressao(node->cond, env);
        int verdadeiro = (cond.type == VAL_BOOL) ? cond.data.b : (cond.type == VAL_DEC ? (cond.data.d != 0) : (cond.data.i != 0));
        if (verdadeiro) {
            executa(node->body, env);
        } else if (node->alt) {
            executa(node->alt, env);
        }
    }
    else if (node->type == AST_ENQUANTO) {
        while (1) {
            LebreValue cond = avalia_expressao(node->cond, env);
            int verdadeiro = (cond.type == VAL_BOOL) ? cond.data.b : (cond.type == VAL_DEC ? (cond.data.d != 0) : (cond.data.i != 0));
            if (!verdadeiro) break;
            executa(node->body, env);
            if (flag_return) break;
        }
    }
    else if (node->type == AST_LINHA) {
        executa(node->init, env);
        while (1) {
            LebreValue cond = avalia_expressao(node->cond, env);
            int verdadeiro = (cond.type == VAL_BOOL) ? cond.data.b : (cond.type == VAL_DEC ? (cond.data.d != 0) : (cond.data.i != 0));
            if (!verdadeiro) break;
            executa(node->body, env);
            if (flag_return) break;
            executa(node->step, env);
        }
    }
    else if (node->type == AST_LINHAS) {
        LebreValue lista = avalia_expressao(node->right, env);
        if (lista.type == VAL_LISTA) {
            for (int i = 0; i < lista.data.l.tamanho; i++) {
                set_var(env, node->name, lista.data.l.itens[i], 0, 0);
                executa(node->body, env);
                if (flag_return) break;
            }
        }
    }
    else if (node->type == AST_INCLUIR) {
        FILE* f = fopen(node->name, "rb");
        if (f) {
            fseek(f, 0, SEEK_END); long l = ftell(f); fseek(f, 0, SEEK_SET);
            char* b = malloc(l + 1); fread(b, 1, l, f); b[l] = '\0'; fclose(f);
            Token* tks = malloc(sizeof(Token) * 10000); lex(b, tks);
            ASTNode* inc_root = parse(tks); executa(inc_root, env);
            free(tks); free(b);
        } else { char msg[256]; sprintf(msg, "Arquivo %s nao encontrado", node->name); lancar_erro(msg); }
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
        LebreValue d; 
        if (node->right) {
            d = avalia_expressao(node->right, env);
        } else {
            if (node->var_type == T_LISTA) {
                d.type = VAL_LISTA;
                d.data.l.tamanho = 0;
                d.data.l.itens = NULL;
                d.data.l.tipoItem = VAL_TEXTO;
            } else if (node->var_type == T_TEXTO) {
                d.type = VAL_TEXTO;
                d.data.s = strdup("");
            } else if (node->var_type == T_DECIMAL) {
                d.type = VAL_DEC;
                d.data.d = 0.0;
            } else if (node->var_type == T_BOOLEANO) {
                d.type = VAL_BOOL;
                d.data.b = 0;
            } else {
                d.type = VAL_INT;
                d.data.i = 0;
            }
        }
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
    else if (node->type == AST_INCREMENTO || node->type == AST_DECREMENTO) {
        LebreValue var_val = get_var(env, node->name);
        if (var_val.type == VAL_INT) {
            var_val.data.i += (node->type == AST_INCREMENTO ? 1 : -1);
        } else if (var_val.type == VAL_DEC) {
            var_val.data.d += (node->type == AST_INCREMENTO ? 1.0 : -1.0);
        }
        set_var(env, node->name, var_val, 0, node->name[strlen(node->name)-1] == '!');
    }
    else if (node->type == AST_MOSTRAR) {
        LebreValue v = avalia_expressao(node->right, env);
        if (v.type == VAL_INT) printf("%d\n", v.data.i);
        else if (v.type == VAL_DEC) printf("%g\n", v.data.d);
        else if (v.type == VAL_TEXTO) printf("%s\n", v.data.s);
        else if (v.type == VAL_BOOL) printf("%s\n", v.data.b ? "VERDADEIRO" : "FALSO");
    }
}
