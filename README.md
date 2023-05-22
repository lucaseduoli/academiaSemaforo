# MultiThread Academia

Este projeto consiste em um programa multithread com sincronização por semáforos, desenvolvido como parte da disciplina MC504 no primeiro semestre de 2023.

## Funcionalidade

O programa simula o funcionamento de uma academia, utilizando threads para representar os equipamentos e os clientes. O objetivo é demonstrar o processamento das threads no terminal, exibindo o estado atual da academia.

## Compilação e Execução

Para compilar o programa, execute o seguinte comando:

```
make
```

Após a compilação, o programa pode ser executado da seguinte maneira:

```
./academia
```

## Descrição das Funções

### `imprimeAcademia()`

Esta função é responsável pela visualização do processamento das threads no terminal. Ela imprime o estado atual da academia, incluindo as filas de espera dos clientes, os equipamentos e os clientes em exercício.

### `f_equipamento(void *id)`

Esta função é executada pelas threads que representam os equipamentos da academia. Ela recebe como parâmetro um ponteiro para o ID do equipamento e realiza o processamento específico de cada equipamento.

### `f_cliente(void *id)`

Esta função é executada pelas threads que representam os clientes da academia. Ela recebe como parâmetro um ponteiro para o ID do cliente e realiza o processamento específico de cada cliente.

## Variáveis e Estruturas

### Variáveis de Estado

- `estadoC[N_CLIENTES]`: vetor que guarda o estado dos clientes (arrived, fazendo exercício, exit, idle ou waiting).
- `estadoE[N_EQUIPAMENTOS]`: vetor que guarda o estado dos equipamentos (fazendo exercício ou sleeping).
- `estadoFilaCliente[N_EQUIPAMENTOS][N_CADEIRAS]`: matriz que guarda o estado das filas de espera dos clientes (busy ou free).
- `clientesFila[N_EQUIPAMENTOS][N_CADEIRAS]`: matriz que guarda o ID dos clientes nas filas de espera dos equipamentos.
- `clientesEquipamento[N_EQUIPAMENTOS]`: vetor que guarda o ID dos clientes nos equipamentos.

### Semáforos

- `sem_filas[N_EQUIPAMENTOS]`: vetor de semáforos para controle de acesso às filas de espera dos equipamentos.
- `sem_cad_equipamento[N_EQUIPAMENTOS]`: vetor de semáforos para controle de acesso aos equipamentos.
- `sem_exercicio_feito[N_EQUIPAMENTOS]`: vetor de semáforos para controle de exercício concluído nos equipamentos.
- `sem_cliente_fila[N_EQUIPAMENTOS]`: vetor de semáforos para controle de acesso aos clientes nas filas de espera.
- `sem_fila_mexendo[N_EQUIPAMENTOS]`: vetor de semáforos para controle de acesso às filas de espera dos equipamentos.
- `sem_escreve_visor[N_EQUIPAMENTOS]`: vetor de semáforos para controle de escrita no visor.
- `sem_le_visor[N_EQUIPAMENTOS]`: vetor de semáforos para controle de leitura do visor pelos clientes.
