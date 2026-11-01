================================================================================
                    EXTENSÃO LEBRE PARA VISUAL STUDIO CODE
================================================================================

Esta pasta contém a extensão oficial de suporte à sintaxe da linguagem Lebre 
para o Visual Studio Code. Ela fornece coloração de código (Syntax Highlighting) 
e facilidades de digitação para arquivos ".leb".

--------------------------------------------------------------------------------
 RECURSOS INCLUSOS
--------------------------------------------------------------------------------
* Coloração de Sintaxe (Syntax Highlighting):
  - Reconhecimento automático de variáveis locais e globais (iniciadas com #).
  - Destaque especial para constantes (#SCREAM_SNAKE_CASE).
  - Cores específicas para palavras-chave (se, senao, enquanto, linha, classe).
  - Destaque para funções nativas (mostrar, aleatorio, arred, iniciarServidor).
  - Reconhecimento de interpolação de variáveis dentro de textos ("${#var}").
  - Identificação de operadores aritméticos, relacionais, lógicos e ternários.

* Autocompletar e Fechamento Automático (Snippets básicos):
  - Fechamento automático de chaves {}, colchetes [] e parênteses ().
  - Fechamento automático de aspas duplas "".
  - Comentários de linha (//) e bloco (/* */) reconhecidos pelo atalho do editor.

--------------------------------------------------------------------------------
 COMO INSTALAR MANUALMENTE
--------------------------------------------------------------------------------

Para instalar a extensão localmente no seu VS Code, basta copiar a pasta 
inteira "lebre-vscode" para o diretório de extensões do seu usuário.

1. FECHE o Visual Studio Code caso ele esteja aberto.

2. COPIE a pasta "lebre-vscode" para o caminho correspondente ao seu sistema:

   [No Windows]
   C:\Users\SEU_USUARIO\.vscode\extensions\
   (Ou acesse rapidamente digitando %USERPROFILE%\.vscode\extensions\ no 
   Explorador de Arquivos).

   [No macOS ou Linux]
   ~/.vscode/extensions/

3. ABRA o Visual Studio Code novamente.

4. CRIE ou ABRA qualquer arquivo com a extensão ".leb". 
   O VS Code vai reconhecer automaticamente a linguagem Lebre e aplicar 
   a coloração de sintaxe ao seu código.

--------------------------------------------------------------------------------
 COMO TESTAR
--------------------------------------------------------------------------------
Após a instalação, crie um arquivo chamado "teste.leb" e cole o código abaixo. 
Ele deverá aparecer totalmente colorido:

// Teste de sintaxe da Lebre
cons decimal #PI = 3.1415
var int #idade = 20

se (#idade >= 18) {
    mostrar("Maior de idade. PI vale: ${#PI}")
} senao {
    mostrar("Menor de idade")
}

var Objeto #pet = novo Animal()
#pet->comer()
================================================================================