#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ordenacao.h"

int comparar_gen_crescente(const void *a, const void *b) {
    float notaA = ((Aluno*)a)->nota;
    float notaB = ((Aluno*)b)->nota;
    if (notaA < notaB) return -1;
    if (notaA > notaB) return 1;
    return 0;
}

int comparar_gen_decrescente(const void *a, const void *b) {
    float notaA = ((Aluno*)a)->nota;
    float notaB = ((Aluno*)b)->nota;
    if (notaA > notaB) return -1;
    if (notaA < notaB) return 1;
    return 0;
}

void limpar_string(char *str, int tamanho) {
    for (int i = tamanho - 1; i >= 0; i--) {
        if (str[i] == ' ' || str[i] == '\r' || str[i] == '\n') {
            str[i] = '\0';
        } else {
            break;
        }
    }
}

int main() {
    FILE *arq_txt = fopen("PROVAO.TXT", "r");
    if (!arq_txt) {
        printf("Erro: PROVAO.TXT nao encontrado na pasta do projeto.\n");
        return 1;
    }

    printf("A ler PROVAO.TXT para a memoria RAM (aguarde)...\n");

    int capacidade = 500000;
    Aluno *alunos = (Aluno *)malloc(capacidade * sizeof(Aluno));
    if (!alunos) {
        printf("Erro de alocacao de memoria para a base de dados global.\n");
        fclose(arq_txt);
        return 1;
    }

    char linha[150];
    char buffer[55];
    int total = 0;

    while (fgets(linha, sizeof(linha), arq_txt) != NULL) {
        if (total >= capacidade) {
            capacidade += 50000;
            alunos = realloc(alunos, capacidade * sizeof(Aluno));
        }

        // Extracao fiel baseada nas posicoes do seu utils.c
        // 1. Inscricao (0 a 8)
        strncpy(buffer, linha + 0, 8); buffer[8] = '\0';
        alunos[total].inscricao = atol(buffer);

        // 2. Nota (9 a 14) -> tamanho 5
        strncpy(buffer, linha + 9, 5); buffer[5] = '\0';
        alunos[total].nota = atof(buffer);

        // 3. Estado (15 a 17) -> tamanho 2
        strncpy(alunos[total].estado, linha + 15, 2); 
        alunos[total].estado[2] = '\0';

        // 4. Cidade (18 a 68) -> tamanho 50
        strncpy(alunos[total].cidade, linha + 18, 50); 
        alunos[total].cidade[50] = '\0';
        limpar_string(alunos[total].cidade, 50);

        // 5. Curso (69 a 99) -> tamanho 30
        strncpy(alunos[total].curso, linha + 69, 30); 
        alunos[total].curso[30] = '\0';
        limpar_string(alunos[total].curso, 30);

        total++;
    }
    fclose(arq_txt);

    printf("Sucesso! %d registros importados.\n", total);

    // Salva o Desordenado Global
    printf("A gravar provao_desordenado.bin...\n");
    FILE *f_des = fopen("provao_desordenado.bin", "wb");
    fwrite(alunos, sizeof(Aluno), total, f_des);
    fclose(f_des);

    // Ordena e Salva o Crescente Global
    printf("A ordenar e gravar provao_crescente.bin...\n");
    qsort(alunos, total, sizeof(Aluno), comparar_gen_crescente);
    FILE *f_cre = fopen("provao_crescente.bin", "wb");
    fwrite(alunos, sizeof(Aluno), total, f_cre);
    fclose(f_cre);

    // Ordena e Salva o Decrescente Global
    printf("A ordenar e gravar provao_decrescente.bin...\n");
    qsort(alunos, total, sizeof(Aluno), comparar_gen_decrescente);
    FILE *f_dec = fopen("provao_decrescente.bin", "wb");
    fwrite(alunos, sizeof(Aluno), total, f_dec);
    fclose(f_dec);

    free(alunos);
    printf("\nPronto! Os 3 superarquivos globais (.bin) foram criados com sucesso.\n");
    return 0;
}