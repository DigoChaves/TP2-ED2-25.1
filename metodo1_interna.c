#include "ordenacao.h"
#include <stdio.h>
#include <stdlib.h>

// Função de comparação para o qsort interno (ordenação por nota crescente)
// Esta função incrementa a variável global de comparações!
int comparar_alunos_metodo1(const void *a, const void *b) {
    metricas_atuais.comparacoes++; 
    float notaA = ((Aluno*)a)->nota;
    float notaB = ((Aluno*)b)->nota;
    if (notaA < notaB) return -1;
    if (notaA > notaB) return 1;
    return 0;
}

void intercalacao_balanceada_interna(int quantidade, int situacao, int print_flag) {
    FILE *arq_bin = fopen("dados.dat", "rb");
    if (!arq_bin) {
        printf("Erro ao abrir dados.dat no Metodo 1.\n");
        return;
    }

    // --- IMPRESSÃO ANTES DA ORDENAÇÃO ---
    if (print_flag) {
        printf("\n--- REGISTOS ANTES DA ORDENACAO ---\n");
        Aluno a;
        for (int i = 0; i < (quantidade > 10 ? 10 : quantidade); i++) {
            if (fread(&a, sizeof(Aluno), 1, arq_bin) == 1) {
                printf("Inscricao: %08ld | Nota: %5.1f\n", a.inscricao, a.nota);
            }
        }
    }
    
    // Zera métricas e volta o ficheiro ao início ANTES de começar a contar a ordenação
    metricas_atuais.transferencias_leitura = 0;
    metricas_atuais.transferencias_escrita = 0;
    metricas_atuais.comparacoes = 0;
    rewind(arq_bin);

    // ========================================================
    // FASE 1: GERAÇÃO DE BLOCOS E DISTRIBUIÇÃO NAS 20 FITAS
    // ========================================================
    
    FILE *fitas[40] = {NULL}; // Fitas 0 a 19 (Entrada) | Fitas 20 a 39 (Saída)
    char nome[20];
    
    // Abre as 20 primeiras fitas para gravar os blocos iniciais
    for (int i = 0; i < 20; i++) {
        sprintf(nome, "fita_%d.dat", i);
        fitas[i] = fopen(nome, "wb");
    }

    Aluno memoria[20]; // Memória RAM rigorosamente limitada a 20 registros
    int lidos = 0;
    int fita_atual = 0;
    int num_blocos = 0;

    // Lê de 20 em 20, ordena na RAM e distribui nas fitas
    while (lidos < quantidade) {
        int qtd_bloco = 0;
        
        // Enche a memória com até 20 registros
        while (qtd_bloco < 20 && lidos < quantidade) {
            if (fread(&memoria[qtd_bloco], sizeof(Aluno), 1, arq_bin) == 1) {
                metricas_atuais.transferencias_leitura++;
                qtd_bloco++;
                lidos++;
            }
        }
        
        if (qtd_bloco > 0) {
            // Ordenação Interna (Qsort)
            qsort(memoria, qtd_bloco, sizeof(Aluno), comparar_alunos_metodo1);
            
            // Grava o bloco ordenado na fita correspondente
            fwrite(memoria, sizeof(Aluno), qtd_bloco, fitas[fita_atual]);
            metricas_atuais.transferencias_escrita += qtd_bloco;
            
            fita_atual = (fita_atual + 1) % 20; // Alterna entre as 20 fitas
            num_blocos++;
        }
    }
    
    fclose(arq_bin);
    for (int i = 0; i < 20; i++) {
        fclose(fitas[i]);
        fitas[i] = NULL;
    }

    // ========================================================
    // FASE 2: INTERCALAÇÃO BALANCEADA DOS BLOCOS
    // ========================================================
    
    int tamanho_bloco = 20;
    int direcao = 0; // 0 = Lê de 0-19 e Grava em 20-39 | 1 = Lê de 20-39 e Grava em 0-19

    // Só precisamos intercalar se houver mais de 1 bloco
    while (num_blocos > 1) {
        int in_offset = (direcao == 0) ? 0 : 20;
        int out_offset = (direcao == 0) ? 20 : 0;

        // Abre fitas de entrada (leitura) e saída (escrita) para a passada atual
        for (int i = 0; i < 20; i++) {
            sprintf(nome, "fita_%d.dat", i + in_offset);
            fitas[i + in_offset] = fopen(nome, "rb");

            sprintf(nome, "fita_%d.dat", i + out_offset);
            fitas[i + out_offset] = fopen(nome, "wb");
        }

        int blocos_gerados = 0;
        fita_atual = 0;
        int fitas_com_dados = 20;

        // Enquanto houver blocos para ler nas fitas de entrada...
        while (fitas_com_dados > 0) {
            Aluno mem_intercalacao[20];
            int lidos_bloco[20] = {0};  // Conta quantos registros já lemos do bloco atual de cada fita
            int tem_registro[20] = {0}; // Indica se há um registro válido aguardando para ser intercalado

            fitas_com_dados = 0;
            
            // Lê o primeiro elemento do bloco atual de cada uma das 20 fitas
            for (int i = 0; i < 20; i++) {
                if (fread(&mem_intercalacao[i], sizeof(Aluno), 1, fitas[i + in_offset]) == 1) {
                    metricas_atuais.transferencias_leitura++;
                    tem_registro[i] = 1;
                    lidos_bloco[i] = 1;
                    fitas_com_dados++;
                }
            }

            if (fitas_com_dados == 0) break; // Todas as fitas acabaram

            // Intercala os elementos do bloco atual
            while (1) {
                int idx_menor = -1;
                
                // Acha a menor nota entre os registros disponíveis na memória
                for (int i = 0; i < 20; i++) {
                    if (tem_registro[i]) {
                        if (idx_menor == -1) {
                            idx_menor = i;
                        } else {
                            metricas_atuais.comparacoes++;
                            if (mem_intercalacao[i].nota < mem_intercalacao[idx_menor].nota) {
                                idx_menor = i;
                            }
                        }
                    }
                }

                // Se não achou nenhum menor, o bloco atual terminou de ser intercalado
                if (idx_menor == -1) break; 

                // Escreve o menor registro encontrado na fita de saída da vez
                fwrite(&mem_intercalacao[idx_menor], sizeof(Aluno), 1, fitas[fita_atual + out_offset]);
                metricas_atuais.transferencias_escrita++;
                tem_registro[idx_menor] = 0; // Marca como consumido

                // Repõe a memória com o próximo elemento da mesma fita de onde saiu o menor (se o bloco não tiver acabado)
                if (lidos_bloco[idx_menor] < tamanho_bloco) {
                    if (fread(&mem_intercalacao[idx_menor], sizeof(Aluno), 1, fitas[idx_menor + in_offset]) == 1) {
                        metricas_atuais.transferencias_leitura++;
                        tem_registro[idx_menor] = 1;
                        lidos_bloco[idx_menor]++;
                    }
                }
            }
            
            blocos_gerados++;
            fita_atual = (fita_atual + 1) % 20; // Próximo bloco vai para a próxima fita de saída
        }

        // Fecha todas as fitas ao final desta passada
        for (int i = 0; i < 40; i++) {
            if (fitas[i]) {
                fclose(fitas[i]);
                fitas[i] = NULL;
            }
        }

        num_blocos = blocos_gerados; // Atualiza a quantidade de blocos para a próxima passada
        tamanho_bloco *= 20;         // O tamanho do bloco gerado é 20 vezes maior
        direcao = 1 - direcao;       // Inverte as fitas (Entrada vira Saída, Saída vira Entrada)
    }

    // --- IMPRESSÃO DO RESULTADO FINAL ---
    if (print_flag) {
        // A fita que contém o arquivo 100% ordenado é a Fita 0 do lado em que ocorreu a última escrita
        int arquivo_final = (num_blocos <= 1 && direcao == 0) ? 0 : ((1 - direcao) == 0 ? 20 : 0);
        sprintf(nome, "fita_%d.dat", arquivo_final);
        
        FILE *arq_final = fopen(nome, "rb");
        if (arq_final) {
            printf("\n--- REGISTOS APOS A ORDENACAO ---\n");
            Aluno a;
            for (int i = 0; i < (quantidade > 10 ? 10 : quantidade); i++) {
                if (fread(&a, sizeof(Aluno), 1, arq_final) == 1) {
                    printf("Inscricao: %08ld | Nota: %5.1f\n", a.inscricao, a.nota);
                }
            }
            fclose(arq_final);
        }
    }
}
