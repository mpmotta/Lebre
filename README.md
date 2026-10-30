
# Lebre 🐇

**Lebre** é uma linguagem de programação interpretada, de tipagem forte e sintaxe baseada na língua portuguesa. Desenvolvida estruturalmente do zero em **C** (sem dependências de runtimes pesados), foi projetada para ser clara, semântica e contar com suporte nativo à construção de APIs e manipulação de bancos de dados relacionais.

Os arquivos de script da linguagem utilizam a extensão `.leb`.

---

## 🚀 Características Principais

* **Motor Próprio em C:** Arquitetura limpa e rápida com analisadores léxico, sintático, semântico (tipagem forte) e executor em tempo real.
* **Convenções Rigorosas:** Padrões visuais estritos garantidos por regras de nomenclatura obrigatórias em nível de compilação para variáveis, constantes e classes.
* **Servidor HTTP e Sockets Nativos:** Capacidade de subir servidores web, escutar portas, receber requisições e enviar respostas (com suporte a CORS e JSON) utilizando apenas os sockets do SO.
* **Integração com Banco de Dados:** Conexão nativa via protocolo TCP com MySQL utilizando arquivos de configuração externos.
* **Multiplataforma:** Suporte total e otimizado para macOS e Windows.

---

## 🛠️ Compilação e Execução

O motor da Lebre é compilado diretamente a partir dos arquivos fonte em C. No diretório raiz, execute:

### No macOS

```bash
clang *.c -o lebre -lreadline

```

### No Windows (via MinGW64)

```cmd
mingw64\bin\gcc.exe *.c -o lebre.exe -lws2_32

```

### Rodando a Linguagem

* **Terminal Interativo (REPL):** Digite `./lebre` (macOS) ou `lebre.exe` (Windows).
* **Executando um Script:** Digite `./lebre script.leb`.

*Dica: Ao modularizar sua API em subpastas, entre na pasta e chame o executável do diretório pai para manter a integridade dos caminhos.*

```bash
cd api
../lebre api.leb

```

---

## 📌 1. Regras de Nomenclatura e Escopo

A Lebre força um padrão estrito de escrita para garantir padronização. A quebra dessas regras gera erro instantâneo no Analisador Semântico:

* **Variáveis e Métodos:** Devem iniciar com `#` seguido por *camelCase*. Ex: `#idade`, `#nomeUsuario`.
* **Constantes:** Devem iniciar com `#` seguido por *SCREAM_SNAKE_CASE*. Ex: `#TAXA_JUROS`.
* **Classes:** Devem ser em *PascalCase* (sem o prefixo `#`). Ex: `Escola`.
* **Escopo Global:** Para declarar uma variável global a partir de um escopo local, utiliza-se o sufixo `!` antes do ponto e vírgula. Ex: `#contadorAcessos!`.

---

## 🗂️ 2. Tipos de Dados Suportados

A Lebre possui **tipagem forte**. Tentativas de reatribuição indevida de tipos (ex: salvar texto em variável inteira) são bloqueadas pelo interpretador.

* `int`: Números inteiros (ex: `10`, `-5`).
* `decimal`: Números de ponto flutuante (ex: `15.75`).
* `texto`: Cadeias de caracteres entre aspas duplas (ex: `"Ola Mundo"`).
* `booleano`: Valores lógicos (`VERDADEIRO` ou `FALSO`).
* `lista`: Arrays dinâmicos gerenciados automaticamente.
* `data`: Tipo especial para formatação e manipulação temporal.

---

## 📝 3. Declaração de Variáveis e Constantes

Utilize as palavras-chave `var` e `cons`.

```leb
var int #idade = 53
var texto #nomeUsuario = "Marcio"
cons decimal #TAXA_JUROS = 1.5
var int #contadorAcessos! = 0

```

---

## ⚙️ 4. Operadores

* **Matemáticos:** `+` (Soma/Concatenação), `-` (Subtração), `*` (Multiplicação), `/` (Divisão), `%` (Módulo), `^` (Potência), `~` (Raiz).
* **Incremento/Decremento:** `++`, `--`.
* **Relacionais:** `==` (Igual), `===` (Idêntico), `!=` (Diferente), `>`, `<`, `>=`, `<=`.
* **Lógicos:** `e` (AND), `ou` (OR).

---

## 🔄 5. Estruturas de Controle e Repetição

### Condicionais (`se` / `senao se` / `senao`)

```leb
var int #nota = 8

se (#nota >= 7) {
    mostrar("Aprovado")
} senao se (#nota >= 5) {
    mostrar("Recuperacao")
} senao {
    mostrar("Reprovado")
}

```

### Laço Fixo (`linha`) e Condicional (`enquanto`)

```leb
linha (var int #i = 0; #i < 10; #i++) {
    mostrar(#i)
}

var int #x = 0
enquanto (#x < 5) {
    mostrar(#x)
    #x++
}

```

---

## 📋 6. Listas (Arrays)

Possui suporte a laços `linhas` (foreach) e operações nativas.

```leb
var lista #alunos
adicionar(#alunos, "Ana")
adicionar(#alunos, "Beto")
adicionar(#alunos, "Carlos")

linhas (var texto #aluno : #alunos) {
    mostrar(#aluno)
}

#alunos[0] = "Ana Maria"
remover(#alunos, 1)
ordenar(#alunos)

```

---

## 🧩 7. Funções de Usuário

```leb
funcao calcularDobro(var int #valor) {
    var int #resultado = #valor * 2
    retorna #resultado
}

var int #total = calcularDobro(15)
mostrar(#total)

```

---

## 🛠️ 8. Funções Nativas do Sistema

| Função | Descrição |
| --- | --- |
| `mostrar(#var)` | Imprime o valor no terminal. |
| `carregar()` | Lê uma entrada de texto do teclado via terminal. |
| `aleatorio(#min, #max)` | Retorna um número inteiro randômico entre os limites. |
| `dividir(#txt, #sep)` | Divide um texto e retorna uma lista baseada no separador. |
| `paraInt(#var)` | Converte o valor para inteiro. |
| `paraDecimal(#var)` | Converte o valor para decimal. |
| `paraTexto(#var)` | Converte o valor para texto. |
| `ordenar(#lista)` | Ordena uma lista alfabeticamente ou numericamente. |
| `media(#val1, ...)` | Calcula a média de argumentos ou de uma lista. |
| `soma(#val1, ...)` | Soma argumentos ou os valores de uma lista. |
| `maximo(#val1, ...)` | Retorna o maior valor. |
| `minimo(#val1, ...)` | Retorna o menor valor. |
| `contar(#var)` | Retorna a quantidade de itens (lista) ou caracteres (texto). |

---

## 📁 9. Leitura, Escrita e Modularização

```leb
salvarArquivo("relatorio.txt", "Dados exportados com sucesso!")
var texto #conteudo = lerArquivo("relatorio.txt")

incluir "biblioteca_matematica.leb"

```

---

## 🌐 10. Servidor Web Nativo, API e Banco de Dados

A Lebre expõe as funções `iniciarServidor`, `receberRequisicao`, `enviarResposta` e `mysqlConsultar` para criar backends inteiros usando Arquitetura MVC modularizada.

**`api.leb`**

```leb
incluir "controller.leb"

iniciarServidor(8000)

enquanto (verdadeiro) {
    processarRotas()
}

```

**`controller.leb`**

```leb
incluir "model.leb"

funcao processarRotas() {
    var texto #req = receberRequisicao()
    var texto #json = mysqlConsultar("SELECT * FROM " + #TABELA_USUARIOS)
    enviarResposta(#json)
}

```

**`model.leb`**

```leb
cons texto #TABELA_USUARIOS = "usuarios"

```

---

## 📄 Licença

```eof

A Lebre é um projeto de código aberto e totalmente livre para uso educacional.
Instituições de ensino, professores e estudantes têm permissão irrestrita para utilizar, modificar, estudar e distribuir o motor da linguagem
e seus códigos-fonte gratuitamente para fins acadêmicos, de aprendizado e pesquisa educacional.

```
