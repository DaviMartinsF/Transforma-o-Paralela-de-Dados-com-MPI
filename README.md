# Transformação Paralela de Vetor com MPI

Este projeto demonstra um padrão básico de computação paralela "Map" (mapear/transformar) usando MPI (Message Passing Interface).

O programa inicializa um vetor de `N` elementos no processo mestre (rank 0), distribui porções desse vetor para todos os processos disponíveis, aplica uma transformação em paralelo (elevar ao quadrado) e, em seguida, coleta os resultados de volta no processo mestre para validação.

## 📝 Descrição do Processo

O fluxo de trabalho segue o modelo Scatter-Gather:

1.  **Inicialização:** O processo 0 aloca e preenche um vetor `original_data` com `DATA_SIZE` elementos (1 a 100).
2.  **Distribuição (Scatter):** O processo 0 usa `MPI_Scatter` para dividir `original_data` em `P` pedaços iguais e envia um pedaço para cada processo (incluindo ele mesmo).
3.  **Transformação (Map):** Cada processo (`id` de 0 a P-1) recebe sua porção em `local_data` e aplica a função `transform_data` (calculando $x^2$) em seus elementos locais.
4.  **Coleta (Gather):** Cada processo envia seu vetor `local_data` transformado de volta para o processo 0, que os reúne em ordem no vetor `gathered_data` usando `MPI_Gather`.
5.  **Validação:** O processo 0 calcula sequencialmente o resultado esperado e o compara com o `gathered_data` recebido do processamento paralelo. Ele imprime "Sucesso" ou "Falha".

## 🛠️ Requisitos

* Um compilador C (como `gcc`).
* Uma implementação do MPI (como **Open MPI**).

## 🚀 Como Compilar

Para compilar o programa, utilize o compilador MPI wrapper `mpicc`:

```bash
mpicc -o transforma_mpi transforma_mpi.c
```
## 🛠️ Como Compilar e Executar
**Compilaçao** 
Use mpirun para executar o programa, especificando o número de processos (-np) desejado.
```bash
mpirun -np <numero_de_processos> ./transforma_mpi

```
**Exemplo com 4 processos**
```bash
mpirun -np 4 ./transforma_mpi

```
**Saida Esperada: **
```bash
Sucesso! O resultado paralelo foi validado corretamente.

```
## ❗ Importante: Limitações da Execução
1. **Divisibilidade de processos**
Esta versão do código usa `MPI_Scatter` e `MPI_Gather` simples, que exigem que os dados possam ser divididos igualmente entre os processos.

* DATA_SIZE (definido no código como 100) deve ser perfeitamente divisível pelo número de processos (P) que você usa no mpirun.

Valores de `-np` que funcionarão: `1, 2, 4, 5, 10, 20, 25, 50, 100`

Valores de `-np` que falharão (intencionalmente): `3, 6, 7, 8, ...`

Se você tentar usar um número não-divisor (ex: 3), o programa irá abortar com a mensagem de erro que implementamos:
```bash
Erro: DATA_SIZE (100) não é divisível por P (3)
MPI_ABORT was invoked on rank 0...

```

2. **"Not enough slots" (Oversubscribing)**
Por padrão, o MPI não permite que você execute mais processos do que o número de núcleos físicos da sua CPU (chamados de "slots"). Se você tem 4 núcleos e tenta rodar com -np 5, você verá este erro:

```bash
There are not enough slots available in the system...

```
Para forçar o MPI a executar mais processos do que os núcleos disponíveis (útil para testes), use a flag `--oversubscribe`:

```bash
#Exemplo para rodar com 5 processos em uma máquina de 4 núcleos
mpirun -np 5 --oversubscribe ./transforma_mpi

```

## ✒️ Autor
* Davi Martins Figueiredo - 10374878
