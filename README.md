# Lebre 🐇

**Lebre** é uma linguagem de programação interpretada, estruturada em C, projetada para ser clara, com tipagem forte e regras rígidas de nomenclatura e escopo. Seus arquivos utilizam a extensão `.leb`.

---

## 🚀 Características Principais

* **Motor Próprio em C:** Desenvolvida do zero, sem dependências de runtimes pesados.
* **Tipagem Forte e Convenções Rigorosas:** Padrões visuais estritos garantidos por regras de nomenclatura de variáveis, constantes e classes.
* **Servidor HTTP e Sockets Nativos:** Capacidade de subir servidores web, escutar portas, receber requisições e enviar respostas (com suporte a CORS e JSON) utilizando apenas os sockets do sistema operacional.
* **Integração com Banco de Dados:** Conexão nativa via protocolo TCP com MySQL utilizando arquivos de configuração externos (`conexao.json`).
* **Multiplataforma:** Suporte total e otimizado para **macOS** e **Windows** (`mingw64`).

---

## 🛠️ Compilação e Instalação

O motor da Lebre é compilado diretamente a partir dos arquivos fonte em C:

### No macOS

```bash
clang *.c -o lebre -lreadline

```

### No Windows (via MinGW64)

```cmd
mingw64\bin\gcc.exe *.c -o lebre.exe -lws2_32

```

---

## 📌 1. Regras de Nomenclatura e Escopo

A Lebre força um padrão estrito de escrita no código para garantir a padronização visual. A quebra dessas regras gera erro de compilação ou execução:

* **Variáveis e Métodos:** Devem iniciar com `#` seguido por `camelCase` (primeira letra minúscula). Ex: `#idade`, `#nomeUsuario`.
* **Constantes:** Devem iniciar com `#` seguido por `SCREAM_SNAKE_CASE` (tudo maiúsculo com sublinhados). Ex: `#TAXA_JUROS`.
* **Classes:** O nome da classe deve ser em `PascalCase` (primeira letra maiúscula, sem o caractere `#`). Ex: `Escola`.
* **Escopo Global:** Adicionar uma exclamação `!` ao final do nome da variável antes do ponto e vírgula a torna global. Ex: `var int #contadorAcessos! = 0;`.

---

## 🗂️ 2. Declaração de Variáveis e Constantes

Utilize as palavras-chave `var` e `cons` seguidas do tipo de dado e do identificador:

```leb
var int #idade = 53;
var texto #nomeUsuario = "Marcio";
cons decimal #TAXA_JUROS = 1.5;
var int #contadorAcessos! = 0; // Global

```

---

## 📊 3. Tipos de Dados Suportados

* `int`: Números inteiros (ex: `10`, `-5`).
* `decimal`: Números de ponto flutuante (ex: `15.75`).
* `texto`: Cadeias de caracteres (ex: `"Ola Mundo"`).
* `booleano`: Valores lógicos (`VERDADEIRO` ou `FALSO`).
* `lista`: Arrays tipados dinâmicos.
* `data`: Formatação em tempo real de data e hora local.

---

## ⚙️ 4. Operadores

### Matemáticos:

* Soma / Concatenação: `+`
* Subtração: `-`
* Multiplicação: `*`
* Divisão: `/`
* Módulo (Resto): `%`
* Potência: `^` (ex: `2 ^ 3`)
* Raiz: `$` (ex: `9 $ 2`)
* Incremento e Decremento: `++`, `--`

### Relacionais e Lógicos:

* Comparação: `==`, `!=`, `>`, `<`, `>=`, `<=`
* Idêntico: `===`
* Lógicos: `E`, `OU`, `NAO`

---

## 🔄 5. Estruturas de Controle e Repetição

### Condicionais (`se` / `senao se` / `senao`):

```leb
se (#nota >= 7) {
    mostrar("Aprovado");
} senao se (#nota >= 5) {
    mostrar("Recuperacao");
} senao {
    mostrar("Reprovado");
}

```

### Laço Fixo (`linha`):

```leb
linha (var int #i = 0; #i < 10; #i++) {
    mostrar(#i);
}

```

### Laço por Condição (`enquanto`):

```leb
var int #x = 0;
enquanto (#x < 5) {
    #x++;
}

```

---

## 📋 6. Listas (Arrays)

```leb
var lista(texto) #alunos = ("Ana", "Beto", "Carlos");

linhas (var texto #aluno : #alunos) {
    mostrar(#aluno);
}

#alunos[0] = "Ana Maria";
adicionar(#alunos, "Daniel");
remover(#alunos, 1);
ordenar(#alunos);

```

---

## 🛠️ 7. Funções Nativas do Sistema

* `mostrar(valor)`
* `carregar()`
* `paraInt(valor)`, `paraDecimal(valor)`, `paraTexto(valor)`
* `aleatorio(min, max)`
* `media()`, `soma()`, `maximo()`, `minimo()`, `contar()`
* `dividir(texto, separador)`

---

## 🧩 8. Funções de Usuário

```leb
funcao #calcularDobro(int #valor) {
    var int #resultado = #valor * 2;
    retorna #resultado;
}

```

---

## 📁 9. Leitura, Escrita e Modularização

```leb
salvarArquivo("relatorio.txt", "Dados exportados com sucesso!");
var texto #conteudo = lerArquivo("relatorio.txt");
incluir "biblioteca_matematica.leb";

```

---

## 🌐 10. Servidor Web Nativo e API (`.leb`)

```leb
iniciarServidor(8080);

enquanto (verdadeiro) {
    var texto #req = receberRequisicao();
    mostrar("Requisição recebida: " + #req);
    enviarResposta("{\"status\": \"sucesso\", \"mensagem\": \"Processado pela Lebre\"}");
}

```

---

## 📄 Licença

Este projeto é distribuído sob os termos da licença especificada no repositório.
