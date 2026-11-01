#include "lebre.h"

int eh_scream_snake(const char* str) {
    if (str[0] != '#') return 0;
    for (int i = 1; str[i] != '\0'; i++) {
        if (str[i] == '!' && str[i+1] == '\0') continue;
        if (!isupper((unsigned char)str[i]) && !isdigit((unsigned char)str[i]) && str[i] != '_') {
            return 0;
        }
    }
    return 1;
}

void lex(const char* src, Token* tokens) {
    int i = 0, t = 0;

    if ((unsigned char)src[0] == 0xEF && (unsigned char)src[1] == 0xBB && (unsigned char)src[2] == 0xBF) {
        i += 3;
    }

    while (src[i] != '\0') {
        if (isspace((unsigned char)src[i])) {
            i++;
            continue;
        }
        if (src[i] == '/' && src[i+1] == '/') {
            while (src[i] != '\0' && src[i] != '\n') i++;
            continue;
        }
        if (src[i] == '/' && src[i+1] == '*') {
            i += 2;
            while (src[i] != '\0' && !(src[i] == '*' && src[i+1] == '/')) i++;
            if (src[i] != '\0') i += 2;
            continue;
        }
        if (src[i] == '"') {
            i++;
            int k = 0;
            while (src[i] != '\0' && src[i] != '"') {
                if (src[i] == '\\' && src[i+1] != '\0') {
                    i++;
                    if (src[i] == 'n') tokens[t].value[k++] = '\n';
                    else if (src[i] == 't') tokens[t].value[k++] = '\t';
                    else tokens[t].value[k++] = src[i];
                    i++;
                } else {
                    tokens[t].value[k++] = src[i++];
                }
            }
            tokens[t].value[k] = '\0';
            if (src[i] == '"') i++;
            tokens[t++].type = T_STRING;
            continue;
        }
        
        if ((unsigned char)src[i] > 127) {
            i++;
            continue;
        }

        if (isdigit((unsigned char)src[i]) || (src[i] == '-' && isdigit((unsigned char)src[i+1]) && (t == 0 || (tokens[t-1].type != T_NUM_INT && tokens[t-1].type != T_NUM_DEC && tokens[t-1].type != T_IDENTIFICADOR && tokens[t-1].type != T_SCREAM_SNAKE && tokens[t-1].type != T_FECHA_PAR && tokens[t-1].type != T_FECHA_COL)))) {
            int k = 0, is_dec = 0;
            if (src[i] == '-') tokens[t].value[k++] = src[i++];
            while (isdigit((unsigned char)src[i]) || src[i] == '.') {
                if (src[i] == '.') is_dec = 1;
                tokens[t].value[k++] = src[i++];
            }
            tokens[t].value[k] = '\0';
            tokens[t++].type = is_dec ? T_NUM_DEC : T_NUM_INT;
            continue;
        }
        if (src[i] == '#' || isalpha((unsigned char)src[i]) || src[i] == '_') {
            int k = 0;
            if (src[i] == '#') tokens[t].value[k++] = src[i++];
            while (isalnum((unsigned char)src[i]) || src[i] == '_') {
                tokens[t].value[k++] = src[i++];
            }
            if (src[i] == '!') {
                tokens[t].value[k++] = src[i++];
            }
            tokens[t].value[k] = '\0';

            if (tokens[t].value[0] == '#') {
                tokens[t].type = eh_scream_snake(tokens[t].value) ? T_SCREAM_SNAKE : T_IDENTIFICADOR;
                t++;
                continue;
            }

            char* v = tokens[t].value;
            if (strcmp(v, "var") == 0) tokens[t].type = T_VAR;
            else if (strcmp(v, "cons") == 0) tokens[t].type = T_CONS;
            else if (strcmp(v, "int") == 0) tokens[t].type = T_INT;
            else if (strcmp(v, "decimal") == 0) tokens[t].type = T_DECIMAL;
            else if (strcmp(v, "texto") == 0) tokens[t].type = T_TEXTO;
            else if (strcmp(v, "booleano") == 0) tokens[t].type = T_BOOLEANO;
            else if (strcmp(v, "lista") == 0) tokens[t].type = T_LISTA;
            else if (strcmp(v, "data") == 0) tokens[t].type = T_DATA;
            else if (strcmp(v, "classe") == 0) tokens[t].type = T_CLASSE;
            else if (strcmp(v, "abstrata") == 0) tokens[t].type = T_ABSTRATA;
            else if (strcmp(v, "estende") == 0) tokens[t].type = T_ESTENDE;
            else if (strcmp(v, "publico") == 0) tokens[t].type = T_PUBLICO;
            else if (strcmp(v, "privado") == 0) tokens[t].type = T_PRIVADO;
            else if (strcmp(v, "protegido") == 0) tokens[t].type = T_PROTEGIDO;
            else if (strcmp(v, "Objeto") == 0) tokens[t].type = T_OBJETO;
            else if (strcmp(v, "novo") == 0) tokens[t].type = T_NOVO;
            else if (strcmp(v, "VERDADEIRO") == 0 || strcmp(v, "verdadeiro") == 0) tokens[t].type = T_VERDADEIRO;
            else if (strcmp(v, "FALSO") == 0 || strcmp(v, "falso") == 0) tokens[t].type = T_FALSO;
            else if (strcmp(v, "se") == 0) tokens[t].type = T_SE;
            else if (strcmp(v, "senao") == 0) tokens[t].type = T_SENAO;
            else if (strcmp(v, "enquanto") == 0) tokens[t].type = T_ENQUANTO;
            else if (strcmp(v, "linha") == 0) tokens[t].type = T_LINHA;
            else if (strcmp(v, "linhas") == 0) tokens[t].type = T_LINHAS;
            else if (strcmp(v, "funcao") == 0) tokens[t].type = T_FUNCAO;
            else if (strcmp(v, "retorna") == 0) tokens[t].type = T_RETORNA;
            else if (strcmp(v, "incluir") == 0) tokens[t].type = T_INCLUIR;
            else if (strcmp(v, "mostrar") == 0) tokens[t].type = T_MOSTRAR;
            else if (strcmp(v, "carregar") == 0) tokens[t].type = T_CARREGAR;
            else if (strcmp(v, "adicionar") == 0) tokens[t].type = T_ADICIONAR;
            else if (strcmp(v, "remover") == 0) tokens[t].type = T_REMOVER;
            else if (strcmp(v, "ordenar") == 0) tokens[t].type = T_ORDENAR;
            else if (strcmp(v, "lerArquivo") == 0) tokens[t].type = T_LER_ARQUIVO;
            else if (strcmp(v, "salvarArquivo") == 0) tokens[t].type = T_SALVAR_ARQUIVO;
            else if (strcmp(v, "aleatorio") == 0) tokens[t].type = T_ALEATORIO;
            else if (strcmp(v, "dividir") == 0) tokens[t].type = T_DIVIDIR;
            else if (strcmp(v, "paraInt") == 0) tokens[t].type = T_PARA_INT;
            else if (strcmp(v, "paraDecimal") == 0) tokens[t].type = T_PARA_DECIMAL;
            else if (strcmp(v, "paraTexto") == 0) tokens[t].type = T_PARA_TEXTO;
            else if (strcmp(v, "media") == 0) tokens[t].type = T_MEDIA;
            else if (strcmp(v, "soma") == 0) tokens[t].type = T_SOMA;
            else if (strcmp(v, "maximo") == 0) tokens[t].type = T_MAXIMO;
            else if (strcmp(v, "minimo") == 0) tokens[t].type = T_MINIMO;
            else if (strcmp(v, "contar") == 0) tokens[t].type = T_CONTAR;
            else if (strcmp(v, "e") == 0) tokens[t].type = T_E;
            else if (strcmp(v, "ou") == 0) tokens[t].type = T_OU;
            else tokens[t].type = T_IDENTIFICADOR;
            t++;
            continue;
        }

        if (strncmp(&src[i], "===", 3) == 0) { strcpy(tokens[t].value, "==="); tokens[t++].type = T_IDENTICO; i += 3; continue; }
        if (strncmp(&src[i], "==", 2) == 0) { strcpy(tokens[t].value, "=="); tokens[t++].type = T_IGUAL; i += 2; continue; }
        if (strncmp(&src[i], "!=", 2) == 0 || strncmp(&src[i], "<>", 2) == 0) { strcpy(tokens[t].value, "!="); tokens[t++].type = T_DIFERENTE; i += 2; continue; }
        if (strncmp(&src[i], ">=", 2) == 0) { strcpy(tokens[t].value, ">="); tokens[t++].type = T_MAIOR_IGUAL; i += 2; continue; }
        if (strncmp(&src[i], "<=", 2) == 0) { strcpy(tokens[t].value, "<="); tokens[t++].type = T_MENOR_IGUAL; i += 2; continue; }
        if (strncmp(&src[i], "->", 2) == 0) { strcpy(tokens[t].value, "->"); tokens[t++].type = T_SETA; i += 2; continue; }
        if (strncmp(&src[i], "&&", 2) == 0) { strcpy(tokens[t].value, "&&"); tokens[t++].type = T_E; i += 2; continue; }
        if (strncmp(&src[i], "||", 2) == 0) { strcpy(tokens[t].value, "||"); tokens[t++].type = T_OU; i += 2; continue; }
        if (strncmp(&src[i], "++", 2) == 0) { strcpy(tokens[t].value, "++"); tokens[t++].type = T_INCREMENTO; i += 2; continue; }
        if (strncmp(&src[i], "--", 2) == 0) { strcpy(tokens[t].value, "--"); tokens[t++].type = T_DECREMENTO; i += 2; continue; }
        if (strncmp(&src[i], "**", 2) == 0) { strcpy(tokens[t].value, "**"); tokens[t++].type = T_POTENCIA; i += 2; continue; }
        if (strncmp(&src[i], "//", 2) == 0) { i += 2; continue; }

        char c = src[i++];
        tokens[t].value[0] = c;
        tokens[t].value[1] = '\0';
        switch (c) {
            case '=': tokens[t++].type = T_ATRIBUICAO; break;
            case '+': tokens[t++].type = T_SOMA_OP; break;
            case '-': tokens[t++].type = T_SUBTRACAO; break;
            case '*': tokens[t++].type = T_MULTIPLICACAO; break;
            case '/': tokens[t++].type = T_DIVISAO; break;
            case '%': tokens[t++].type = T_MODULO; break;
            case '^': tokens[t++].type = T_POTENCIA; break;
            case '~': tokens[t++].type = T_RAIZ; break;
            case '>': tokens[t++].type = T_MAIOR; break;
            case '<': tokens[t++].type = T_MENOR; break;
            case '(': tokens[t++].type = T_ABRE_PAR; break;
            case ')': tokens[t++].type = T_FECHA_PAR; break;
            case '{': tokens[t++].type = T_ABRE_CHAVE; break;
            case '}': tokens[t++].type = T_FECHA_CHAVE; break;
            case '[': tokens[t++].type = T_ABRE_COL; break;
            case ']': tokens[t++].type = T_FECHA_COL; break;
            case ';': tokens[t++].type = T_PONTO_VIRGULA; break;
            case ',': tokens[t++].type = T_VIRGULA; break;
            case ':': tokens[t++].type = T_DOIS_PONTOS; break;
            case '.': tokens[t++].type = T_PONTO; break;
            default: tokens[t++].type = T_DESCONHECIDO; break;
        }
    }
    tokens[t].type = T_FIM;
    strcpy(tokens[t].value, "");
}