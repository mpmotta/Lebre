================================================================================
                    GUIA DE IMPLEMENTAÇÃO DE API COM LEBRE
================================================================================

Este diretório contém o exemplo base de uma API Web conectada a um banco de 
dados MySQL, construída 100% com as funções nativas da linguagem Lebre, 
sem a necessidade de frameworks ou servidores externos como Apache ou Nginx.

A arquitetura utilizada segue o padrão MVC (Model-View-Controller) simplificado 
para garantir um código limpo e modularizado.

--------------------------------------------------------------------------------
 ESTRUTURA DE ARQUIVOS
--------------------------------------------------------------------------------

* api.leb        -> Arquivo principal (Entry Point). É ele quem inicia o 
                    servidor na porta desejada e mantém o laço de repetição 
                    infinito (enquanto verdadeiro) escutando requisições.

* controller.leb -> Camada Lógica (Controller). Responsável por receber a 
                    requisição, interpretar a rota, solicitar dados ao banco 
                    e enviar a resposta final (geralmente em formato JSON).

* model.leb      -> Camada de Dados (Model). Guarda as constantes de banco 
                    de dados (como nomes de tabelas) e isola as regras 
                    específicas de consulta (Query).

--------------------------------------------------------------------------------
 FUNÇÕES NATIVAS UTILIZADAS
--------------------------------------------------------------------------------

- iniciarServidor(#porta)
  Abre um socket de rede TCP nativo no Sistema Operacional para escutar conexões.

- receberRequisicao()
  Trava a execução (bloqueio síncrono) até que um cliente (Navegador, Postman) 
  faça um request, retornando o corpo da requisição HTTP como texto.

- mysqlConsultar(#query)
  Conecta ao banco MySQL via socket TCP (usando o driver nativo em C da Lebre) 
  e retorna os registros formatados em JSON puro.

- enviarResposta(#conteudo)
  Empacota o conteúdo com cabeçalhos HTTP válidos (incluindo CORS) e devolve 
  ao cliente que originou a requisição.

--------------------------------------------------------------------------------
 COMO EXECUTAR
--------------------------------------------------------------------------------

1. Certifique-se de que o banco de dados MySQL está rodando na sua máquina e 
   que as credenciais no arquivo do motor C (ou variáveis de ambiente, 
   dependendo da sua compilação) estão corretas.

2. Pelo terminal, navegue até a pasta onde estão os arquivos da API.

3. Execute o arquivo principal chamando o binário da Lebre:
   (Lembre-se de apontar corretamente para o executável se ele estiver na 
   pasta raiz, por exemplo: ../lebre api.leb)

   No macOS/Linux:
   ../lebre api.leb

   No Windows:
   ..\lebre.exe api.leb

4. O terminal ficará "travado" (escutando). 
   Abra seu navegador ou Postman e acesse:
   http://localhost:8000

5. Para derrubar o servidor, pressione CTRL+C no terminal.

================================================================================