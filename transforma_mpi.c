

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define DATA_SIZE 100  // Tamanho total do vetor de dados

// Função que aplica a transformação: eleva o número ao quadrado
int transform_data(int x) {
    return x * x;
}

int main() {
    int id, P;  // id = rank do processo, P = número total de processos

    MPI_Init(NULL, NULL);  // Inicializa o ambiente MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &id);  // Obtém o rank (ID) do processo atual
    MPI_Comm_size(MPI_COMM_WORLD, &P);   // Obtém o número total de processos

    // --- Verificação de Divisibilidade ---
    // Garante que os dados possam ser divididos igualmente
    if (id == 0 && DATA_SIZE % P != 0) {
        fprintf(stderr, "Erro: DATA_SIZE (%d) não é divisível por P (%d)\n", DATA_SIZE, P);
        // Sinaliza aos outros processos para terminarem
        MPI_Abort(MPI_COMM_WORLD, 1); 
    }

    // Vetor local que armazenará os dados recebidos por cada processo
    int local_data_size = DATA_SIZE / P;
    int local_data[local_data_size];

    int* original_data = NULL;  // Ponteiro para o vetor original (só usado pelo processo 0)
    int* gathered_data = NULL;  // Ponteiro para o vetor final transformado (só no processo 0)

    // Processo 0 inicializa os dados originais e aloca memória para o resultado
    if (id == 0) {
        original_data = (int *)malloc(sizeof(int) * DATA_SIZE);
        for (int i = 0; i < DATA_SIZE; ++i) {
            original_data[i] = i + 1;  // Preenche com valores de 1 a 100
        }

        gathered_data = (int *)malloc(sizeof(int) * DATA_SIZE);  // Aloca espaço para o vetor transformado
    }

    // Distribui partes iguais do vetor original para todos os processos
    MPI_Scatter(
        original_data,       // vetor a ser distribuído (apenas no processo 0)
        local_data_size,     // quantidade de elementos por processo
        MPI_INT,             // tipo dos dados
        local_data,          // buffer local onde os dados serão recebidos
        local_data_size, MPI_INT,  // mesmo tipo e quantidade
        0, MPI_COMM_WORLD   // processo raiz (0) e comunicador
    );

    // Aplica a transformação localmente: x -> x²
    for (int i = 0; i < local_data_size; ++i) {
        local_data[i] = transform_data(local_data[i]);
    }

    // Recolhe os dados transformados de todos os processos no processo 0
    MPI_Gather(
        local_data,          // dados transformados de cada processo
        local_data_size, MPI_INT,  // tipo e quantidade
        gathered_data,       // buffer final (apenas no processo 0)
        local_data_size, MPI_INT,
        0, MPI_COMM_WORLD
    );

    // Processo 0 imprime os resultados E VALIDA
    if (id == 0) {
        
        // --- SEÇÃO DE VALIDAÇÃO ---
        int validacao_ok = 1; // Flag: 1 = OK, 0 = Erro
        for (int i = 0; i < DATA_SIZE; i++) {
            int resultado_esperado = transform_data(original_data[i]);
            if (gathered_data[i] != resultado_esperado) {
                printf("Erro na validação! Posição %d: Esperado %d, Recebido %d\n", 
                       i, resultado_esperado, gathered_data[i]);
                validacao_ok = 0;
                break; // Para no primeiro erro encontrado
            }
        }

        if (validacao_ok) {
            printf("Sucesso! O resultado paralelo foi validado corretamente.\n");
        } else {
            printf("Falha! O resultado paralelo está incorreto.\n");
        }
        // -----------------------------------------------------------


        // Libera a memória alocada no processo 0
        free(original_data);
        free(gathered_data);
    }

    MPI_Finalize();  // Finaliza o ambiente MPI
    return 0;
}