#include "lebre.h"
#include <mysql/mysql.h>

MYSQL *conn = NULL;

LebreValue func_mysqlConectar(Ambiente* env, ASTNode** args, int arg_count) {
    LebreValue val; val.type = VAL_BOOL; val.data.b = 0;
    if (arg_count < 4) { lancar_erro("mysqlConectar exige 4 argumentos: host, usuario, senha, banco"); return val; }
    
    LebreValue host = avalia_expressao(args[0], env);
    LebreValue user = avalia_expressao(args[1], env);
    LebreValue pass = avalia_expressao(args[2], env);
    LebreValue db = avalia_expressao(args[3], env);
    
    if (conn) { mysql_close(conn); conn = NULL; }
    conn = mysql_init(NULL);
    if (!conn) { lancar_erro("Erro ao inicializar MySQL"); return val; }
    
    if (!mysql_real_connect(conn, host.data.s, user.data.s, pass.data.s, db.data.s, 0, NULL, 0)) {
        lancar_erro(mysql_error(conn)); return val;
    }
    val.data.b = 1;
    return val;
}

LebreValue func_mysqlConsultar(Ambiente* env, ASTNode** args, int arg_count) {
    LebreValue val; val.type = VAL_TEXTO; val.data.s = strdup("[]");
    if (arg_count < 1) { lancar_erro("mysqlConsultar exige a query SQL"); return val; }
    if (!conn) { lancar_erro("Nenhuma conexao MySQL ativa"); return val; }
    
    LebreValue query = avalia_expressao(args[0], env);
    if (mysql_query(conn, query.data.s)) { lancar_erro(mysql_error(conn)); return val; }
    
    MYSQL_RES *result = mysql_store_result(conn);
    if (!result) return val; // INSERT/UPDATE/DELETE
    
    int num_campos = mysql_num_fields(result);
    MYSQL_FIELD *campos = mysql_fetch_fields(result);
    MYSQL_ROW row;
    
    char* json = malloc(1024 * 1024); // Buffer de 1MB inicial
    strcpy(json, "[");
    int primeira_linha = 1;
    
    while ((row = mysql_fetch_row(result))) {
        if (!primeira_linha) strcat(json, ",");
        strcat(json, "{");
        for (int i = 0; i < num_campos; i++) {
            if (i > 0) strcat(json, ",");
            char ch_key[256];
            sprintf(ch_key, "\"%s\":", campos[i].name);
            strcat(json, ch_key);
            
            if (row[i] == NULL) {
                strcat(json, "null");
            } else {
                int is_numeric = (campos[i].type == MYSQL_TYPE_DECIMAL || campos[i].type == MYSQL_TYPE_TINY || 
                                  campos[i].type == MYSQL_TYPE_SHORT || campos[i].type == MYSQL_TYPE_LONG ||
                                  campos[i].type == MYSQL_TYPE_FLOAT || campos[i].type == MYSQL_TYPE_DOUBLE ||
                                  campos[i].type == MYSQL_TYPE_LONGLONG || campos[i].type == MYSQL_TYPE_INT24);
                
                if (is_numeric) {
                    strcat(json, row[i]);
                } else {
                    strcat(json, "\"");
                    // Simplificado: idealmente escapar aspas internas
                    strcat(json, row[i]);
                    strcat(json, "\"");
                }
            }
        }
        strcat(json, "}");
        primeira_linha = 0;
    }
    strcat(json, "]");
    mysql_free_result(result);
    
    val.data.s = strdup(json);
    free(json);
    return val;
}

LebreValue func_mysqlFechar(Ambiente* env, ASTNode** args, int arg_count) {
    if (conn) { mysql_close(conn); conn = NULL; }
    LebreValue val; val.type = VAL_BOOL; val.data.b = 1;
    return val;
}

void registrar_modulo_mysql(Ambiente* env) {
    LebreValue v1; v1.type = VAL_FUNCAO_C; v1.data.func_c = func_mysqlConectar;
    set_var(env, "mysqlConectar", v1, 1, 1);
    
    LebreValue v2; v2.type = VAL_FUNCAO_C; v2.data.func_c = func_mysqlConsultar;
    set_var(env, "mysqlConsultar", v2, 1, 1);
    
    LebreValue v3; v3.type = VAL_FUNCAO_C; v3.data.func_c = func_mysqlFechar;
    set_var(env, "mysqlFechar", v3, 1, 1);
}
