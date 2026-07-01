#include "ordenacao.h"
#include <stdlib.h>
#include <string.h>

// --- Funções Auxiliares de Leitura e Escrita (TXT mantidas caso precises noutro local) ---

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
    
    if (strlen(linha) < 50) return ler_aluno_txt(arquivo, aluno); 

    extrair_campo(linha, 0, 8, buffer);
    aluno->inscricao = atol(buffer);

    extrair_campo(linha, 9, 5, buffer);
    aluno->nota = atof(buffer);

    extrair_campo(linha, 15, 2, aluno->estado);
    extrair_campo(linha, 18, 50, aluno->cidade);
    extrair_campo(linha, 69, 30, aluno->curso);

    return 1;
}

// --- Funções de Manipulação Binária para as Métricas ---

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

// --- Geração do Ambiente de Teste (Cópia Direta de Bloco Binário) ---

void gerar_arquivo_trabalho(int quantidade, int situacao) {
    char nome_arquivo[50];

    // 1. Define qual ficheiro binário abrir com base na situação
    if (situacao == 1) {
        strcpy(nome_arquivo, "provao_crescente.bin");
        printf("A extrair dados do arquivo: %s...\n", nome_arquivo);
    } else if (situacao == 2) {
        strcpy(nome_arquivo, "provao_decrescente.bin");
        printf("A extrair dados do arquivo: %s...\n", nome_arquivo);
    } else {
        strcpy(nome_arquivo, "provao_desordenado.bin"); 
        printf("A extrair dados do arquivo: %s...\n", nome_arquivo);
    }

    FILE *arq_fonte = fopen(nome_arquivo, "rb");
    FILE *arq_bin = fopen("dados.bin", "wb");
    
    if (!arq_fonte) {
        printf("Erro Critico: O arquivo '%s' nao foi encontrado na pasta!\n", nome_arquivo);
        exit(1);
    }
    if (!arq_bin) {
        printf("Erro ao criar o ficheiro temporario dados.bin\n");
        fclose(arq_fonte);
        exit(1);
    }

    // 2. Aloca memória para transferir os dados de uma só vez (muito mais rápido)
    Aluno *buffer = (Aluno *)malloc(quantidade * sizeof(Aluno));
    if (!buffer) {
        printf("Erro de alocacao de memoria para a transferencia.\n");
        fclose(arq_fonte);
        fclose(arq_bin);
        exit(1);
    }

    // 3. Lê exatamente a quantidade pedida do arquivo fonte e escreve no dados.bin
    int lidos = fread(buffer, sizeof(Aluno), quantidade, arq_fonte);
    fwrite(buffer, sizeof(Aluno), lidos, arq_bin);

    // 4. Limpa a casa
    free(buffer);
    fclose(arq_fonte);
    fclose(arq_bin);
    
    if (lidos < quantidade) {
        printf("Aviso: Foram pedidos %d registos, mas o arquivo so tinha %d.\n", quantidade, lidos);
    } else {
        printf("Arquivo de trabalho 'dados.bin' pronto com %d registros.\n", lidos);
    }
}
