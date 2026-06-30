#include "ordenacao.h"
#include <stdlib.h>
#include <string.h>

// --- Funções de Comparação para a Preparação ---
int comparar_notas_crescente(const void *a, const void *b) {
    float notaA = ((Aluno*)a)->nota;
    float notaB = ((Aluno*)b)->nota;
    if (notaA < notaB) return -1;
    if (notaA > notaB) return 1;
    return 0;
}

int comparar_notas_decrescente(const void *a, const void *b) {
    float notaA = ((Aluno*)a)->nota;
    float notaB = ((Aluno*)b)->nota;
    if (notaA > notaB) return -1;
    if (notaA < notaB) return 1;
    return 0;
}

// --- Funções Auxiliares de Leitura e Escrita ---

void extrair_campo(const char *linha, int inicio, int tamanho, char *destino) {
    if (strlen(linha) < inicio + tamanho) {
        destino[0] = '\0';
        return;
    }
    strncpy(destino, linha + inicio, tamanho);
    destino[tamanho] = '\0';
}

int ler_aluno_txt(FILE *arquivo, Aluno *aluno) {
    char linha[150];
    char buffer[55];

    if (fgets(linha, sizeof(linha), arquivo) == NULL) return 0;

    extrair_campo(linha, 0, 8, buffer);
    aluno->inscricao = atol(buffer);

    extrair_campo(linha, 9, 5, buffer);
    aluno->nota = atof(buffer);

    extrair_campo(linha, 15, 2, aluno->estado);
    extrair_campo(linha, 18, 50, aluno->cidade);
    extrair_campo(linha, 69, 30, aluno->curso);

    return 1;
}

void ler_aluno_bin(FILE *arq, int pos, Aluno *a) {
    fseek(arq, pos * sizeof(Aluno), SEEK_SET);
    fread(a, sizeof(Aluno), 1, arq);
    metricas_atuais.transferencias_leitura++;
}

void escrever_aluno_bin(FILE *arq, int pos, Aluno *a) {
    fseek(arq, pos * sizeof(Aluno), SEEK_SET);
    fwrite(a, sizeof(Aluno), 1, arq);
    metricas_atuais.transferencias_escrita++;
}

// --- Geração do Ambiente de Teste ---

// Prepara o ficheiro de trabalho. NÃO CONTA nas métricas de ordenação.
void gerar_arquivo_trabalho(int quantidade, int situacao) {
    FILE *arq_txt = fopen("PROVAO.TXT", "r");
    FILE *arq_bin = fopen("dados.dat", "wb");
    
    if (!arq_txt || !arq_bin) {
        printf("Erro ao criar ficheiro de trabalho inicial.\n");
        exit(1);
    }

    // Aloca memória para carregar todos os alunos pedidos de uma vez
    Aluno *alunos = (Aluno *)malloc(quantidade * sizeof(Aluno));
    if (alunos == NULL) {
        printf("Erro de alocacao de memoria na preparacao do ficheiro!\n");
        exit(1);
    }

    // Lê os registos do ficheiro de texto
    int lidos = 0;
    while (lidos < quantidade && ler_aluno_txt(arq_txt, &alunos[lidos])) {
        lidos++;
    }

    // Aplica a pré-ordenação se o utilizador pediu Situação 1 ou 2
    if (situacao == 1) {
        printf("A pre-ordenar ambiente de teste: Situacao 1 (Crescente)...\n");
        qsort(alunos, lidos, sizeof(Aluno), comparar_notas_crescente);
    } else if (situacao == 2) {
        printf("A pre-ordenar ambiente de teste: Situacao 2 (Decrescente)...\n");
        qsort(alunos, lidos, sizeof(Aluno), comparar_notas_decrescente);
    } else {
        printf("A preparar ambiente de teste: Situacao 3 (Desordenado)...\n");
    }

    // Grava o vetor preparado no ficheiro binário temporário (dados.dat)
    fwrite(alunos, sizeof(Aluno), lidos, arq_bin);

    // Limpa a memória e fecha os ficheiros
    free(alunos);
    fclose(arq_txt);
    fclose(arq_bin);
}