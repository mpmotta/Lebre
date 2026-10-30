#include "lebre.h"
#include <setjmp.h>

#if defined(_WIN32) || defined(_WIN64)
    #define MODO_WINDOWS 1
#else
    #define MODO_WINDOWS 0
    #include <readline/readline.h>
    #include <readline/history.h>
#endif

Ambiente ambiente_global;
LebreValue global_return_val;
int flag_return = 0;
jmp_buf env_erro;
int modo_repl = 0;

void lancar_erro(const char* msg) {
    printf("Erro: %s\n", msg);
    if (modo_repl) {
        longjmp(env_erro, 1);
    } else {
        exit(1);
    }
}

int main(int argc, char** argv) {
    srand((unsigned int)time(NULL));
    ambiente_global.count = 0;
    ambiente_global.pai = NULL;
    
    registrar_modulo_mysql(&ambiente_global);
    registrar_modulo_http(&ambiente_global);

    if (argc == 1) {
        printf("Interpretador Lebre v1.0\n");
        printf("Digite 'sair' para encerrar o terminal.\n");
        modo_repl = 1;
        
        char* volatile linha_lida = NULL;
        Token* volatile tokens = NULL;

        while (1) {
            if (setjmp(env_erro) != 0) {
                if (linha_lida) { free(linha_lida); linha_lida = NULL; }
                if (tokens) { free(tokens); tokens = NULL; }
                continue;
            }

            #if MODO_WINDOWS
                char buffer[2048];
                printf("lebre> ");
                if (!fgets(buffer, sizeof(buffer), stdin)) {
                    break;
                }
                buffer[strcspn(buffer, "\r\n")] = '\0';
                linha_lida = strdup(buffer);
            #else
                linha_lida = readline("lebre> ");
                if (!linha_lida) {
                    break;
                }
                if (strlen(linha_lida) > 0) {
                    add_history(linha_lida);
                }
            #endif

            if (strcmp(linha_lida, "sair") == 0) {
                free(linha_lida);
                break;
            }

            if (strlen(linha_lida) == 0) {
                free(linha_lida);
                linha_lida = NULL;
                continue;
            }

            tokens = malloc(sizeof(Token) * 10000);
            lex(linha_lida, tokens);
            ASTNode* root = parse(tokens);
            analisar_semantica(root);
            executa(root, &ambiente_global);
            
            free(tokens);
            tokens = NULL;
            free(linha_lida);
            linha_lida = NULL;
        }
        return 0;
    }

    modo_repl = 0;
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

    Token* volatile tokens = NULL;
    if (setjmp(env_erro) == 0) {
        tokens = malloc(sizeof(Token) * 10000);
        lex(buffer, tokens);
        ASTNode* root = parse(tokens);
        analisar_semantica(root);
        executa(root, &ambiente_global);
        free(tokens);
    } else {
        if (tokens) free(tokens);
    }

    free(buffer);
    return 0;
}