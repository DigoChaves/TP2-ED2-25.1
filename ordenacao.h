#ifndef ORDENACAO_H
#define ORDENACAO_H

#include <stdio.h>

// --- ESTRUTURAS DE DADOS ---
typedef struct {
    long inscricao;      
    float nota;          
    char estado[3];      
    char cidade[51];     
    char curso[31];      
} Aluno;

typedef struct {
    long transferencias_leitura;
    long transferencias_escrita;
    long comparacoes;
    double tempo_execucao;
} Metricas;

// --- VARIÁVEIS GLOBAIS ---
extern Metricas metricas_atuais;

// --- FUNÇÕES AUXILIARES (utils.c) ---
void extrair_campo(const char *linha, int inicio, int tamanho, char *destino);
int ler_aluno_txt(FILE *arquivo, Aluno *aluno);
void ler_aluno_bin(FILE *arq, int pos, Aluno *a);
void escrever_aluno_bin(FILE *arq, int pos, Aluno *a);

// Gera um arquivo "dados.dat" a partir do PROVAO.TXT com a quantidade desejada
void gerar_arquivo_trabalho(int quantidade, int situacao); 

// --- MÉTODOS DE ORDENAÇÃO ---
void intercalacao_balanceada_interna(int quantidade, int situacao, int print_flag);
void intercalacao_balanceada_substituicao(int quantidade, int situacao, int print_flag);
void quicksort_externo(int quantidade, int situacao, int print_flag);

#endif // ORDENACAO_H
