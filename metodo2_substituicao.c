#include "ordenacao.h"
#include <stdio.h>
#include <stdlib.h>

// --- FUNÇÕES PARA MANIPULAR O MIN-HEAP ---

// Refaz a propriedade do Min-Heap descendo o elemento da posição 'esq'
void refaz_heap(Aluno *H, int esq, int dir) {
    int i = esq;
    int j = i * 2 + 1; // Filho à esquerda
    Aluno aux = H[i];
    
    while (j <= dir) {
        metricas_atuais.comparacoes++;
        if (j < dir && H[j].nota > H[j + 1].nota) {
            j++; // Pega o menor dos filhos
        }
        metricas_atuais.comparacoes++;
        if (aux.nota <= H[j].nota) {
            break; // Já está na posição correta
        }
        H[i] = H[j];
        i = j;
        j = i * 2 + 1;
    }
    H[i] = aux;
}

// Constrói o Min-Heap a partir de um vetor desordenado
void constroi_heap(Aluno *H, int n) {
    int esq = (n / 2) - 1;
    while (esq >= 0) {
        refaz_heap(H, esq, n - 1);
        esq--;
    }
}

// --- MÉTODO 2 ---

void intercalacao_balanceada_substituicao(int quantidade, int situacao, int print_flag) {
    FILE *arq_bin = fopen("dados.dat", "rb");
    if (!arq_bin) {
        printf("Erro ao abrir dados.dat no Metodo 2.\n");
        return;
    }

    if (print_flag) {
        printf("\n--- REGISTOS ANTES DA ORDENACAO ---\n");
        Aluno a;
        for (int i = 0; i < (quantidade > 10 ? 10 : quantidade); i++) {
            if (fread(&a, sizeof(Aluno), 1, arq_bin) == 1) {
                printf("Inscricao: %08ld | Nota: %5.1f\n", a.inscricao, a.nota);
            }
        }
    }
    
    metricas_atuais.transferencias_leitura = 0;
    metricas_atuais.transferencias_escrita = 0;
    metricas_atuais.comparacoes = 0;
    rewind(arq_bin);

    // ========================================================
    // FASE 1: GERAÇÃO DE BLOCOS VARIÁVEIS (SELEÇÃO POR SUBSTITUIÇÃO)
    // ========================================================
    
    FILE *fitas[40] = {NULL};
    char nome[20];
    
    for (int i = 0; i < 20; i++) {
        sprintf(nome, "fita_%d.dat", i);
        fitas[i] = fopen(nome, "wb");
    }

    // Registo Sentinela para marcar o fim de um bloco variável nas fitas
    Aluno sentinela;
    sentinela.inscricao = -1;
    sentinela.nota = -1.0;

    Aluno memoria[20];
    int lidos_total = 0;
    int fita_atual = 0;
    int num_blocos = 0;
    int tamanho_heap = 0;

    // Enche a memória inicial
    while (tamanho_heap < 20 && lidos_total < quantidade) {
        if (fread(&memoria[tamanho_heap], sizeof(Aluno), 1, arq_bin) == 1) {
            metricas_atuais.transferencias_leitura++;
            tamanho_heap++;
            lidos_total++;
        }
    }

    constroi_heap(memoria, tamanho_heap);
    int total_na_memoria = tamanho_heap;

    while (total_na_memoria > 0) {
        // Grava a raiz do Heap (menor elemento) na fita
        fwrite(&memoria[0], sizeof(Aluno), 1, fitas[fita_atual]);
        metricas_atuais.transferencias_escrita++;
        
        Aluno ultimo_gravado = memoria[0];
        Aluno novo_aluno;
        int leu_novo = 0;

        // Tenta ler o próximo elemento
        if (lidos_total < quantidade && fread(&novo_aluno, sizeof(Aluno), 1, arq_bin) == 1) {
            metricas_atuais.transferencias_leitura++;
            lidos_total++;
            leu_novo = 1;
        }

        if (leu_novo) {
            metricas_atuais.comparacoes++;
            if (novo_aluno.nota >= ultimo_gravado.nota) {
                // O novo elemento ainda pertence a este bloco!
                memoria[0] = novo_aluno;
                refaz_heap(memoria, 0, tamanho_heap - 1);
            } else {
                // O novo elemento pertence ao PRÓXIMO bloco. Fica retido no fim do vetor.
                memoria[0] = memoria[tamanho_heap - 1];
                memoria[tamanho_heap - 1] = novo_aluno;
                tamanho_heap--; // O heap encolhe virtualmente
                if (tamanho_heap > 0) {
                    refaz_heap(memoria, 0, tamanho_heap - 1);
                }
            }
        } else {
            // Acabou o ficheiro, apenas encolhe o heap
            memoria[0] = memoria[tamanho_heap - 1];
            tamanho_heap--;
            total_na_memoria--;
            if (tamanho_heap > 0) {
                refaz_heap(memoria, 0, tamanho_heap - 1);
            }
        }

        // Se o heap esvaziou, o bloco atual acabou!
        if (tamanho_heap == 0 && total_na_memoria > 0) {
            fwrite(&sentinela, sizeof(Aluno), 1, fitas[fita_atual]); // Marca o fim do bloco
            fita_atual = (fita_atual + 1) % 20; // Muda de fita
            num_blocos++;
            tamanho_heap = total_na_memoria; // Restaura o tamanho do heap com os elementos retidos
            constroi_heap(memoria, tamanho_heap);
        }
    }
    
    // Marca o fim do último bloco
    fwrite(&sentinela, sizeof(Aluno), 1, fitas[fita_atual]);
    num_blocos++;
    
    fclose(arq_bin);
    for (int i = 0; i < 20; i++) {
        fclose(fitas[i]);
        fitas[i] = NULL;
    }

    // ========================================================
    // FASE 2: INTERCALAÇÃO BALANCEADA DOS BLOCOS VARIÁVEIS
    // ========================================================
    
    int direcao = 0;

    while (num_blocos > 1) {
        int in_offset = (direcao == 0) ? 0 : 20;
        int out_offset = (direcao == 0) ? 20 : 0;

        for (int i = 0; i < 20; i++) {
            sprintf(nome, "fita_%d.dat", i + in_offset);
            fitas[i + in_offset] = fopen(nome, "rb");

            sprintf(nome, "fita_%d.dat", i + out_offset);
            fitas[i + out_offset] = fopen(nome, "wb");
        }

        int blocos_gerados = 0;
        fita_atual = 0;
        int fitas_ativas = 20;

        while (fitas_ativas > 0) {
            Aluno mem_intercalacao[20];
            int tem_registro[20] = {0};
            fitas_ativas = 0;
            
            // Carrega o primeiro elemento de cada fita
            for (int i = 0; i < 20; i++) {
                if (fread(&mem_intercalacao[i], sizeof(Aluno), 1, fitas[i + in_offset]) == 1) {
                    if (mem_intercalacao[i].inscricao != -1) {
                        metricas_atuais.transferencias_leitura++;
                        tem_registro[i] = 1;
                        fitas_ativas++;
                    }
                }
            }

            if (fitas_ativas == 0) break; // Todas as fitas acabaram

            while (1) {
                int idx_menor = -1;
                
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

                if (idx_menor == -1) break; // Bloco interalado com sucesso
                
                // Grava o menor
                fwrite(&mem_intercalacao[idx_menor], sizeof(Aluno), 1, fitas[fita_atual + out_offset]);
                metricas_atuais.transferencias_escrita++;
                tem_registro[idx_menor] = 0;

                // Tenta ler o próximo da mesma fita
                if (fread(&mem_intercalacao[idx_menor], sizeof(Aluno), 1, fitas[idx_menor + in_offset]) == 1) {
                    if (mem_intercalacao[idx_menor].inscricao == -1) {
                        // Encontrou a sentinela, o bloco nesta fita acabou
                        tem_registro[idx_menor] = 0;
                    } else {
                        metricas_atuais.transferencias_leitura++;
                        tem_registro[idx_menor] = 1;
                    }
                }
            }
            
            fwrite(&sentinela, sizeof(Aluno), 1, fitas[fita_atual + out_offset]); // Marca o fim do novo bloco gerado
            blocos_gerados++;
            fita_atual = (fita_atual + 1) % 20;
        }

        for (int i = 0; i < 40; i++) {
            if (fitas[i]) {
                fclose(fitas[i]);
                fitas[i] = NULL;
            }
        }

        num_blocos = blocos_gerados;
        direcao = 1 - direcao;
    }

    // --- IMPRESSÃO DO RESULTADO FINAL ---
    if (print_flag) {
        int arquivo_final = (num_blocos <= 1 && direcao == 0) ? 0 : ((1 - direcao) == 0 ? 20 : 0);
        sprintf(nome, "fita_%d.dat", arquivo_final);
        
        FILE *arq_final = fopen(nome, "rb");
        if (arq_final) {
            printf("\n--- REGISTOS APOS A ORDENACAO ---\n");
            Aluno a;
            int contador = 0;
            // Lê ignorando sentinelas (embora o ficheiro final seja apenas um bloco gigante)
            while (fread(&a, sizeof(Aluno), 1, arq_final) == 1 && contador < (quantidade > 10 ? 10 : quantidade)) {
                if (a.inscricao != -1) {
                    printf("Inscricao: %08ld | Nota: %5.1f\n", a.inscricao, a.nota);
                    contador++;
                }
            }
            fclose(arq_final);
        }
    }
}