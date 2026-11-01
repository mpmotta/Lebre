#include "lebre.h"

#if defined(_WIN32) || defined(_WIN64)
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
#endif

int server_fd = -1;
int client_socket = -1;

LebreValue func_iniciarServidor(Ambiente* env, ASTNode** args, int arg_count) {
    LebreValue val; val.type = VAL_BOOL; val.data.b = 0;
    if (arg_count < 1) { lancar_erro("iniciarServidor exige a porta"); return val; }
    
    LebreValue porta = avalia_expressao(args[0], env);
    
    #if defined(_WIN32) || defined(_WIN64)
        WSADATA wsa;
        if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) { lancar_erro("Falha no Winsock"); return val; }
    #endif

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) { lancar_erro("Erro ao criar socket"); return val; }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(porta.data.i);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        lancar_erro("Erro no bind do servidor HTTP"); return val;
    }

    if (listen(server_fd, 10) < 0) {
        lancar_erro("Erro ao escutar porta"); return val;
    }
    
    val.data.b = 1;
    return val;
}

LebreValue func_receberRequisicao(Ambiente* env, ASTNode** args, int arg_count) {
    LebreValue val; val.type = VAL_TEXTO; val.data.s = strdup("");
    if (server_fd == -1) { lancar_erro("Servidor nao iniciado"); return val; }

    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    client_socket = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
    
    if (client_socket < 0) { lancar_erro("Erro ao aceitar conexao"); return val; }

    char buffer[4096] = {0};
    #if defined(_WIN32) || defined(_WIN64)
        recv(client_socket, buffer, 4095, 0);
    #else
        read(client_socket, buffer, 4095);
    #endif

    val.data.s = strdup(buffer);
    return val;
}

LebreValue func_enviarResposta(Ambiente* env, ASTNode** args, int arg_count) {
    LebreValue val; val.type = VAL_BOOL; val.data.b = 0;
    if (client_socket == -1) { lancar_erro("Nenhum cliente conectado"); return val; }
    if (arg_count < 1) { lancar_erro("enviarResposta exige o corpo do JSON"); return val; }

    LebreValue corpo = avalia_expressao(args[0], env);
    char resposta[8192];
    sprintf(resposta, "HTTP/1.1 200 OK\r\nContent-Type: application/json; charset=UTF-8\r\nAccess-Control-Allow-Origin: *\r\nContent-Length: %lu\r\n\r\n%s", strlen(corpo.data.s), corpo.data.s);

    #if defined(_WIN32) || defined(_WIN64)
        send(client_socket, resposta, strlen(resposta), 0);
        closesocket(client_socket);
    #else
        write(client_socket, resposta, strlen(resposta));
        close(client_socket);
    #endif

    client_socket = -1;
    val.data.b = 1;
    return val;
}

void registrar_modulo_http(Ambiente* env) {
    LebreValue v1; v1.type = VAL_FUNCAO_C; v1.data.func_c = func_iniciarServidor;
    set_var(env, "iniciarServidor", v1, 1, 1, T_PUBLICO);
    
    LebreValue v2; v2.type = VAL_FUNCAO_C; v2.data.func_c = func_receberRequisicao;
    set_var(env, "receberRequisicao", v2, 1, 1, T_PUBLICO);
    
    LebreValue v3; v3.type = VAL_FUNCAO_C; v3.data.func_c = func_enviarResposta;
    set_var(env, "enviarResposta", v3, 1, 1, T_PUBLICO);
}