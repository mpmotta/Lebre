Documentacao Oficial da Linguagem Lebre (v1.0)

A linguagem Lebre e uma ferramenta interpretada, estruturada em C, projetada para ser clara, com tipagem forte e regras rigidas de nomenclatura. Seus arquivos utilizam a extensao .leb.

1. Regras de Nomenclatura e Escopo
A Lebre forca um padrao de escrita no codigo para garantir a padronizacao visual. A quebra dessas regras gera erro de compilacao/execucao.
- Variaveis e Metodos: Devem iniciar com # seguido por camelCase (primeira minuscula).
- Constantes: Devem iniciar com # seguido por SCREAM_SNAKE_CASE (tudo maiusculo com subtracos).
- Classes: O nome da classe deve ser PascalCase (primeira maiuscula, sem #).
- Escopo Global: Adicionar uma exclamacao ! ao final do nome da variavel antes do ponto e virgula a torna global.

2. Declaracao de Variaveis e Constantes
Utilize as palavras-chave var e cons seguidas do tipo de dado e do identificador.

var int #idade = 53;
var texto #nomeUsuario = "Marcio";
cons decimal #TAXA_JUROS = 1.5;
var int #contadorAcessos! = 0; // Global

3. Tipos de Dados Suportados
- int: Numeros inteiros (ex: 10, -5).
- decimal: Numeros de ponto flutuante (ex: 15.75).
- texto: Cadeias de caracteres (ex: "Ola Mundo").
- booleano: Valores logicos (VERDADEIRO ou FALSO).
- lista: Arrays tipados dinamicos.
- data: Formatacao em tempo real de data e hora local.

4. Operadores
Matematicos:
- Soma/Concatenacao: +
- Subtracao: -
- Multiplicacao: *
- Divisao: /
- Modulo (Resto): %
- Potencia: ^ (ex: 2 ^ 3)
- Raiz: $ (ex: 9 $ 2)
- Incremento e Decremento: ++, --

Relacionais e Logicos:
- Comparacao: ==, !=, >, <, >=, <=
- Identico: ===
- Logicos: E, OU, NAO

5. Estruturas de Controle e Repeticao
Condicionais (se / senao se / senao):
se (#nota >= 7) {
    mostrar("Aprovado");
} senao {
    mostrar("Reprovado");
}

Laco Fixo (linha):
linha (var int #i = 0; #i < 10; #i++) {
    mostrar(#i);
}

Laco por Condicao (enquanto):
var int #x = 0;
enquanto (#x < 5) {
    #x++;
}

6. Listas (Arrays)
Declaracao e Laco (linhas):
var lista(texto) #alunos = ("Ana", "Beto", "Carlos");
linhas (var texto #aluno : #alunos) {
    mostrar(#aluno);
}

Manipulacao:
#alunos[0] = "Ana Maria";
adicionar(#alunos, "Daniel");
remover(#alunos, 1);
ordenar(#alunos);

7. Funcoes Nativas do Sistema
- mostrar(valor)
- carregar()
- paraInt(valor), paraDecimal(valor), paraTexto(valor)
- aleatorio(min, max)
- media(), soma(), maximo(), minimo(), contar()
- dividir(texto, separador)

8. Funcoes de Usuario
funcao #calcularDobro(int #valor) {
    var int #resultado = #valor * 2;
    retorna #resultado;
}

9. Leitura, Escrita e Modularizacao
salvarArquivo("relatorio.txt", "Dados exportados com sucesso!");
var texto #conteudo = lerArquivo("relatorio.txt");
incluir "biblioteca_matematica.leb";