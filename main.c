#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "ordenacao.h"

// Instanciando a variável global de métricas
Metricas metricas_atuais = {0, 0, 0, 0.0};

int main(int argc, char *argv[]) {
    if (argc < 4 || argc > 5) {
        printf("Uso: ordena <metodo> <quantidade> <situacao> [-P]\n");
        return 1;
    }

    int metodo = atoi(argv[1]);
    int quantidade = atoi(argv[2]);
    int situacao = atoi(argv[3]);
    int print_flag = (argc == 5 && strcmp(argv[4], "-P") == 0) ? 1 : 0;

    // 1. Gera o arquivo dados.dat com a quantidade e situação desejada
    printf("Preparando arquivo de trabalho...\n");
    gerar_arquivo_trabalho(quantidade, situacao);

    // 2. Inicia o cronômetro
    clock_t inicio = clock();

    // 3. Chama o método escolhido
    switch (metodo) {
        case 1:
            printf("Executando Intercalacao Balanceada (Ordenacao Interna)...\n");
            intercalacao_balanceada_interna(quantidade, situacao, print_flag);
            break;
        case 2:
            printf("Executando Intercalacao Balanceada (Selecao por Substituicao)...\n");
            intercalacao_balanceada_substituicao(quantidade, situacao, print_flag);
            break;
        case 3:
            printf("Executando Quicksort Externo...\n");
            quicksort_externo(quantidade, situacao, print_flag);
            break;
        default:
            printf("Metodo invalido!\n");
            return 1;
    }

    // 4. Para o cronômetro
    clock_t fim = clock();
    metricas_atuais.tempo_execucao = ((double)(fim - inicio)) / CLOCKS_PER_SEC;

    // 5. Exibe os resultados
    printf("\n--- RESULTADOS DA ANALISE EXPERIMENTAL ---\n");
    printf("Transferencias (Leitura): %ld\n", metricas_atuais.transferencias_leitura);
    printf("Transferencias (Escrita): %ld\n", metricas_atuais.transferencias_escrita);
    printf("Comparacoes: %ld\n", metricas_atuais.comparacoes);
    printf("Tempo de execucao: %f segundos\n", metricas_atuais.tempo_execucao);

    return 0;
}