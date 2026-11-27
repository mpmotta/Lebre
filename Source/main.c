#include "lebre.h"
#include <setjmp.h>
#include <ctype.h>

#if defined(_WIN32) || defined(_WIN64)
    #define MODO_WINDOWS 1
#else
    #define MODO_WINDOWS 0
    #include <readline/readline.h>
    #include <readline/history.h>
#endif

const char* LEBRE_VERSAO = "2.5.1";
Ambiente ambiente_global;
LebreValue global_return_val;
int flag_return = 0;
jmp_buf env_erro;
int modo_repl = 0;

int comparar_sem_caso(const char* a, const char* b) {
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) {
            return 0;
        }
        a++;
        b++;
    }
    return *a == '\0' && *b == '\0';
}

int verificar_comando_cli(int argc, char** argv, const char* cmd) {
    if (argc >= 2 && strncmp(argv[1], "--", 2) == 0 && comparar_sem_caso(argv[1] + 2, cmd)) {
        return 1;
    }
    if (argc >= 3 && strcmp(argv[1], "--") == 0 && comparar_sem_caso(argv[2], cmd)) {
        return 1;
    }
    return 0;
}

void mostrar_author(void) {
    puts("Author: Marcio Paiva da Motta - UniRitter - Nov/2026");
}

void mostrar_acronimo(void) {
    puts("L.E.B.R.E. = Linguagem Encapsulada Basica Rapida e Eficiente");
}

void mostrar_ascii(void) {
    puts("                      ..----------------..");
    puts("                   .-@@@@@@@@@@@@@@@@@@@@@@-.");
    puts("                  @@@@@@@@@@@@@@@@@@@@@@@@@@@@.");
    puts("                  @@@@@@@@@@@---........'@@@@@@.");
    puts("                  '-------'              '@@@@@@@---..");
    puts("                                         .@@@@@@@@@@@@@@.");
    puts("               ..---@@@@@@@@@---..      .@@@@@@@@@@   @@@.");
    puts("           .-@@@@@@@@@@@@@@@@@@@@@-.   .@@@@@@@@@@@   @@@@.");
    puts("        .-@@@@@@@@@@@@@@@@@@@@@@@@@@@-.@@@@@@@@@@@@@@@@@@@@.");
    puts("       .@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
    puts("      .@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@'");
    puts("     .@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@---''");
    puts("     @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@'");
    puts("     @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
    puts("     @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@'");
    puts("  .-. @@@@@@@@@@@@@@@@@@@---..@@@@@@@@@@@@@@@@@@'");
    puts(" .@@@.@@@@@@@@@@@@@@@@@@@@@'  .@@@@@@@@@@@@@@@@'");
    puts(" @@@@@.@@@@@@@@@@@@@@@@@@@@@@ .@@@@@@@@@@@@@@@'");
    puts(" '@@@@@.@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@---'''     ..--.");
    puts("  '@@@@@..@@@@@@@@@@@@@@@@@@@@@@@@@'           .@@@@@@.");
    puts("    '---' '@@@@@@@@@@@@@@@@@@@@@@@@---......--@@@@@@@@'");
    puts("              '---@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@'");
    puts("                .@@@@@@@@@@@@'  '---@@@@@@@@@@@@--'");
    puts("              .@@@@@@@@@@@@'          ''------''");
    puts("             .@@@@@@@@@@@'");
    puts("              '--------'");
}

void lancar_erro(const char* msg) {
    printf("Erro: %s\n", msg);
    if (modo_repl) {
        longjmp(env_erro, 1);
    } else {
        exit(1);
    }
}

int main(int argc, char** argv) {
    if (argc >= 2 && (strcmp(argv[1], "--ver") == 0 || (argc >= 3 && strcmp(argv[1], "--") == 0 && strcmp(argv[2], "ver") == 0))) {
        printf("Lebre %s\n", LEBRE_VERSAO);
        return 0;
    }

    if (verificar_comando_cli(argc, argv, "author")) {
        mostrar_author();
        return 0;
    }

    if (verificar_comando_cli(argc, argv, "ascii")) {
        mostrar_ascii();
        return 0;
    }

    if (verificar_comando_cli(argc, argv, "L.E.B.R.E.")) {
        mostrar_acronimo();
        return 0;
    }

    if (argc >= 2 && argv[1][0] == '-') {
        if (strcmp(argv[1], "--") == 0 && argc >= 3) {
            printf("parametro -- %s invalido.\n", argv[2]);
        } else {
            printf("parametro %s invalido.\n", argv[1]);
        }
        return 1;
    }

    srand((unsigned int)time(NULL));
    ambiente_global.count = 0;
    ambiente_global.pai = NULL;
    
    registrar_modulo_mysql(&ambiente_global);
    registrar_modulo_http(&ambiente_global);

    if (argc == 1) {
        printf("########## Interpretador Lebre v%s ##########\n", LEBRE_VERSAO);
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
                printf("<lebre> ");
                if (!fgets(buffer, sizeof(buffer), stdin)) {
                    break;
                }
                buffer[strcspn(buffer, "\r\n")] = '\0';
                linha_lida = strdup(buffer);
            #else
                linha_lida = readline("<lebre> ");
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

            if (strcmp(linha_lida, "ver") == 0) {
                printf("Lebre %s\n", LEBRE_VERSAO);
                free(linha_lida);
                linha_lida = NULL;
                continue;
            }

            if (comparar_sem_caso(linha_lida, "author")) {
                mostrar_author();
                free(linha_lida);
                linha_lida = NULL;
                continue;
            }

            if (comparar_sem_caso(linha_lida, "ascii")) {
                mostrar_ascii();
                free(linha_lida);
                linha_lida = NULL;
                continue;
            }

            if (comparar_sem_caso(linha_lida, "L.E.B.R.E.")) {
                mostrar_acronimo();
                free(linha_lida);
                linha_lida = NULL;
                continue;
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