#include "ordenacao.h"
#include <stdio.h>

void particao_externa(FILE *arq, int esq, int dir, int *i, int *j) {
    *i = esq;
    *j = dir;
    Aluno pivo, aluno_i, aluno_j;
    
    // Pivô no meio
    ler_aluno_bin(arq, (*i + *j) / 2, &pivo);
    
    while (*i <= *j) {
        ler_aluno_bin(arq, *i, &aluno_i);
        while (1) {
            metricas_atuais.comparacoes++;
            if (aluno_i.nota < pivo.nota) {
                (*i)++;
                ler_aluno_bin(arq, *i, &aluno_i);
            } else {
                break;
            }
        }
        
        ler_aluno_bin(arq, *j, &aluno_j);
        while (1) {
            metricas_atuais.comparacoes++;
            if (aluno_j.nota > pivo.nota) {
                (*j)--;
                ler_aluno_bin(arq, *j, &aluno_j);
            } else {
                break;
            }
        }
        
        if (*i <= *j) {
            escrever_aluno_bin(arq, *i, &aluno_j);
            escrever_aluno_bin(arq, *j, &aluno_i);
            (*i)++;
            (*j)--;
        }
    }
}

void quicksort_externo_rec(FILE *arq, int esq, int dir) {
    int i, j;
    if (esq < dir) {
        particao_externa(arq, esq, dir, &i, &j);
        quicksort_externo_rec(arq, esq, j);
        quicksort_externo_rec(arq, i, dir);
    }
}

void quicksort_externo(int quantidade, int situacao, int print_flag) {
    FILE *arq_bin = fopen("dados.dat", "r+b");
    if (!arq_bin) {
        printf("Erro ao abrir dados.dat no Quicksort.\n");
        return;
    }

    if (print_flag) {
        printf("\n--- REGISTOS ANTES DA ORDENACAO ---\n");
        Aluno a;
        for (int i = 0; i < (quantidade > 10 ? 10 : quantidade); i++) {
            ler_aluno_bin(arq_bin, i, &a);
            printf("Inscricao: %08ld | Nota: %5.1f\n", a.inscricao, a.nota);
        }
    }

    // Zera métricas ANTES de ordenar (caso a leitura com print_flag tenha somado algo)
    metricas_atuais.transferencias_leitura = 0;
    metricas_atuais.transferencias_escrita = 0;
    metricas_atuais.comparacoes = 0;

    // Inicia a ordenação
    quicksort_externo_rec(arq_bin, 0, quantidade - 1);

    if (print_flag) {
        printf("\n--- REGISTOS APOS A ORDENACAO ---\n");
        Aluno a;
        for (int i = 0; i < (quantidade > 10 ? 10 : quantidade); i++) {
            ler_aluno_bin(arq_bin, i, &a);
            printf("Inscricao: %08ld | Nota: %5.1f\n", a.inscricao, a.nota);
        }
    }

    fclose(arq_bin);
}