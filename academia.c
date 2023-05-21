/****************************************************************************
 * File name:        academia.c
 * File description: O arquivo serve o projeto MultiThread da disciplina
 *		     MC504 ministrada no primeiro semestre de 2023.
 *                   O intuito principal e'desenvolvimento multithread com
 *                   sincronizacao por sema'foros, contendo duas threads
 *                   f_equipamento e f_cliente (cada uma com mais de uma
 *                   instancia).
 * Author name:   Giovanna Gennari
 *                Lucas Oliveira
 *                Borges
 *                Rafael Ifanger
 * Creation date:  05/21/2023
 * Revision date:
 ****************************************************************************/

/****************************************************************************
 * Function name:        imprimeAcademia
 * Function description: Visualizacao do processamento das threads no terminal
 * Input parameters:     none
 * Output parameters:    none
 ****************************************************************************/

/****************************************************************************
 * Function name:        f_equipamento
 * Function description: Thread que simboliza o equipamento
 * Input parameters:     void pointer (id do equipamento)
 * Output parameters:    void pointer
 ****************************************************************************/

/****************************************************************************
 * Function name:        f_cliente
 * Function description: Thread que simboliza o cliente
 * Input parameters:     void pointer (id do cliente)
 * Output parameters:    void pointer
 ****************************************************************************/

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N_CLIENTES 20
#define N_EQUIPAMENTOS 3
#define N_CADEIRAS 4

/*
 * Variaveis para guardar estado dos clientes/equipamentos.
 * A - arrived           (cliente);
 * F (C ou B) - fazendo exercicio  (cliente/equipamento);
 * E - exit  		 (cliente);
 * I - idle              (cliente);
 * L - left    		 (cliente);
 * S - sleeping 	 (equipamento);
 * W - waiting		 (cliente).
 */
typedef enum
{
  A,
  FC,
  E,
  I,
  L,
  W
} estado_c;
typedef enum
{
  FB,
  S
} estado_b;

estado_c estadoC[N_CLIENTES];
estado_b estadoB[N_EQUIPAMENTOS];

/*
 * Variaveis para guardar estado das cadeiras.
 * B - busy
 * F - free
 */
typedef enum
{
  B,
  F
} estado_cadeira;

estado_cadeira estadoCadeiraCliente[N_EQUIPAMENTOS][N_CADEIRAS];

int clientesCadeira[N_EQUIPAMENTOS][N_CADEIRAS];
int clienteEquipamento[N_EQUIPAMENTOS];

sem_t sem_ultima_cadeira[N_EQUIPAMENTOS];
sem_t sem_cad_equipamento[N_EQUIPAMENTOS];
sem_t sem_exercicio_feito[N_EQUIPAMENTOS];
sem_t sem_cliente_cadeira[N_EQUIPAMENTOS];
sem_t sem_equipamento_livre[N_EQUIPAMENTOS];
sem_t sem_cadeira_livre[N_EQUIPAMENTOS][N_CADEIRAS];
sem_t sem_fila_mexendo[N_EQUIPAMENTOS];

sem_t sem_cliente_entrou_equipamento[N_EQUIPAMENTOS];
sem_t sem_le_visor[N_EQUIPAMENTOS];

sem_t sem_estados; /* Semaforo para troca de estados. */

int visor[N_EQUIPAMENTOS];

char equipamentos[3][4][21] = {
    {
        {"             /\\    "},
        {"  __________/ /    "},
        {" /__________\\/     "},
        {"//         //      "},
    },
    {{"   _          _    "}, {"  |_|        |_|   "}, {" _//        _//    "}, {"|_|        |_|     "}},
    {{"                   "}, {"  ______________   "}, {" /__________\\___\\  "}, {"//         //      "}},
};

char pessoa[4][5] = {
    {"  _ "},
    {" |_|"},
    {" /|\\"},
    {" / \\"},
};

/* Funcao responsavel pela impressao da academia. */
void imprimeAcademia()
{
  int i, j, qtosClientesArrived = 0, qtosClientesExit = 0, titulo = 0;

  int clientesArrived[N_CLIENTES], clientesExit[N_CLIENTES];

  /* Verifica quais clientes estao no estado "Arrived". */
  for (i = 0; i < N_CLIENTES; i++)
  {
    if (estadoC[i] == A)
    {
      clientesArrived[qtosClientesArrived++] = i;
    }
  }

  /* Verifica quais clientes estao no estado "Exit". */
  for (i = 0; i < N_CLIENTES; i++)
  {
    if (estadoC[i] == E)
    {
      clientesExit[qtosClientesExit++] = i;
    }
  }

  /*****************************************************************************
   **************************** IMPRESSAO DO TITULO ****************************
   ****************************************************************************/

  /* Imprime os espacos em branco da parte referente aos clientes Arrived. */
  printf("\n");
  for (i = N_CLIENTES - (N_EQUIPAMENTOS * N_CADEIRAS) - 1; i >= 0; i--)
  {
    printf("     ");
  }

  /* Faz a conta para imprimir o tí­tulo 'ACADEMIA' no meio. */

  if (N_CLIENTES - (N_EQUIPAMENTOS * N_CADEIRAS) == 0)
  {
    printf("     ");
  }

  titulo = (4 + 6 * N_CADEIRAS + 27) / 2;

  titulo -= 5;

  for (i = 0; i < titulo; i++)
  {
    printf(" ");
  }

  printf("ACADEMIA\n");

  /*****************************************************************************
   ************************** FIM IMPRESSAO DO TITULO **************************
   ****************************************************************************/

  /*****************************************************************************
   ********************** IMPRESSÃO DAS LINHAS INICIAIS DA ACADEMIA  ***********
   ****************************************************************************/
  for (i = N_CLIENTES - (N_EQUIPAMENTOS * N_CADEIRAS) - 1; i >= 0; i--)
  {
    printf("     ");
  }

  /* Se nao chegou clientes, imprimimos espacamento para ficar alinhado com
   * outros momentos da academia.
   */
  if (N_CLIENTES - (N_EQUIPAMENTOS * N_CADEIRAS) == 0)
  {
    printf("     ");
  }
  printf(" ");
  for (i = 0; i < N_CADEIRAS; i++)
  {
    printf("______");
  }
  printf("__________________________\n");

  for (i = N_CLIENTES - (N_EQUIPAMENTOS * N_CADEIRAS) - 1; i >= 0; i--)
  {
    printf("     ");
  }

  /* Se nao chegou clientes, imprimimos espacamento para ficar alinhado com
   * outros momentos da academia.
   */
  if (N_CLIENTES - (N_EQUIPAMENTOS * N_CADEIRAS) == 0)
  {
    printf("     ");
  }
  printf("/");
  for (i = 0; i < N_CADEIRAS; i++)
  {
    printf("      ");
  }
  printf("                          \\\n");

  /*****************************************************************************
   ************************FIM DA IMPRESSÃO DAS LINHAS INICIAIS DA ACADEMIA  ***
   ****************************************************************************/

  for (int k = 0; k < N_EQUIPAMENTOS; k++)
  {
    /*****************************************************************************
     ************************** IMPRESSAO PRIMEIRA LINHA DA ITERAÇÃO *************
     ****************************************************************************/
    for (i = N_CLIENTES - (N_EQUIPAMENTOS * N_CADEIRAS) - 1; i >= 0; i--)
    {
      printf("     ");
    }

    /* Se nao chegou clientes, imprimimos espacamento para ficar alinhado com
     * outros momentos da academia.
     */
    if (N_CLIENTES - (N_EQUIPAMENTOS * N_CADEIRAS) == 0)
    {
      printf("     ");
    }
    printf("|");
    for (i = 0; i < N_CADEIRAS; i++)
    {
      printf("      ");
    }
    printf("                          |\n");

    for (i = N_CLIENTES - (N_EQUIPAMENTOS * N_CADEIRAS) - 1; i >= 0; i--)
    {
      if (k == 0 && i < qtosClientesArrived)
      {
        printf(" ... ");
      }
      else
      {
        printf("     ");
      }
    }

    /* Se nao chegou clientes, imprimimos espacamento para ficar alinhado com
     * outros momentos da academia.
     */
    if (N_CLIENTES - (N_EQUIPAMENTOS * N_CADEIRAS) == 0)
    {
      printf("     ");
    }

    printf("| ");

    /* Imprimi as cadeiras dos clientes. */
    for (i = N_CADEIRAS - 1; i >= 0; i--)
    {
      if (estadoCadeiraCliente[k][i] == B)
      {
        printf("  ... ");
      }
      else
      {
        printf("      ");
      }
    }

    /* EspaÃ§o entre as cadeiras dos clientes e as dos equipamentos. */
    printf("    ");

    switch (estadoB[k])
    {
    case FB:
      printf("       ... ");
      break;
    case S:
      printf("       zzz ");
      break;
    }

    printf("          | \n");

    /*****************************************************************************
     ********************** FIM DA IMPRESSAO PRIMEIRA LINHA DA ITERAÇÃO **********
     ****************************************************************************/

    /*****************************************************************************
     ************************** IMPRESSAO SEGUNDA LINHA DA ITERAÇÃO **************
     ****************************************************************************/

    for (i = N_CLIENTES - (N_EQUIPAMENTOS * N_CADEIRAS) - 1; i >= 0; i--)
    {
      if (k == 0 && i < qtosClientesArrived)
      {
        printf(" %2d  ", clientesArrived[i]);
      }
      else
      {
        printf("     ");
      }
    }

    /* Se nao chegou clientes, imprimimos espacamento para ficar alinhado com
     * outros momentos da academia.
     */
    if (N_CLIENTES - (N_EQUIPAMENTOS * N_CADEIRAS) == 0)
    {
      printf("     ");
    }

    printf("| ");

    for (i = N_CADEIRAS - 1; i >= 0; i--)
    {
      if (estadoCadeiraCliente[k][i] == B)
      {
        printf("  %2d  ", clientesCadeira[k][i]);
      }
      else
      {
        printf("      ");
      }
    }

    /* EspaÃ§o entre as cadeiras dos clientes e dos equipamentos. */
    printf("    ");

    j = 0;
    switch (estadoB[k])
    {
    case FB:
      printf("       %2d  ", clienteEquipamento[k]);
      break;
    case S:
      printf("           ");
      break;
    }

    printf("          | ");

    if (k == N_EQUIPAMENTOS - 1)
    {
      for (i = 0; i < qtosClientesExit; i++)
      {
        printf(" %2d  ", clientesExit[i]);
      }
    }

    printf("\n");

    /*****************************************************************************
     ********************** FIM DA IMPRESSAO SEGUNDA LINHA DA ITERAÇÃO ***********
     ****************************************************************************/

    /*****************************************************************************
     ************************* IMPRESSAO TERCEIRA LINHA DA ITERAÇÃO **************
     ****************************************************************************/

    for (i = N_CLIENTES - (N_EQUIPAMENTOS * N_CADEIRAS) - 1; i >= 0; i--)
    {
      printf("     ");
    }

    /* Se nao chegou clientes, imprimimos espacamento para ficar alinhado com
     * outros momentos da academia.
     */
    if (N_CLIENTES - (N_EQUIPAMENTOS * N_CADEIRAS) == 0)
    {
      printf("     ");
    }

    printf("| ");

    for (i = N_CADEIRAS - 1; i >= 0; i--)
    {
      printf("  F%02d ", i);
    }

    printf("          ");

    printf(" E%02d           ", k);

    printf("| \n");
    for (i = N_CLIENTES - (N_EQUIPAMENTOS * N_CADEIRAS) - 1; i >= 0; i--)
    {
      printf("     ");
    }

    /* Se nao chegou clientes, imprimimos espacamento para ficar alinhado com
     * outros momentos da academia.
     */
    if (N_CLIENTES - (N_EQUIPAMENTOS * N_CADEIRAS) == 0)
    {
      printf("     ");
    }
    printf("|");
    for (i = 0; i < N_CADEIRAS; i++)
    {
      printf("      ");
    }
    printf("                          |\n");

    /*****************************************************************************
     ********************** FIM DA IMPRESSAO TERCEIRA LINHA DA ITERAÇÃO **********
     ****************************************************************************/
    /*****************************************************************************
     ********************** IMPRESSÃO DAS LINHAS DE ASCII DA ITERAÇÃO ************
     ****************************************************************************/
    for (int j = 0; j < 4; j++)
    {
      for (i = N_CLIENTES - (N_EQUIPAMENTOS * N_CADEIRAS) - 1; i >= 0; i--)
      {
        printf("     ");
      }

      /* Se nao chegou clientes, imprimimos espacamento para ficar alinhado com
       * outros momentos da academia.
       */
      if (N_CLIENTES - (N_EQUIPAMENTOS * N_CADEIRAS) == 0)
      {
        printf("     ");
      }

      printf("| ");

      for (i = N_CADEIRAS - 1; i >= 0; i--)
      {
        if (estadoCadeiraCliente[k][i] == B)
          printf(" %s ", pessoa[j]);
        else
        {
          printf("      ");
        }
      }

      printf("    ");

      printf(" %s ", equipamentos[k % 3][j]);

      printf("| \n");
    }
    /*****************************************************************************
     ********************** FIM DA IMPRESSÃO DAS LINHAS DE ASCII DA ITERAÇÃO *****
     ****************************************************************************/
  }
  /*****************************************************************************
   ********************** IMPRESSÃO DA LINHA FINAL DA ACADEMIA  ****************
   ****************************************************************************/
  for (i = N_CLIENTES - (N_EQUIPAMENTOS * N_CADEIRAS) - 1; i >= 0; i--)
  {
    printf("     ");
  }

  /* Se nao chegou clientes, imprimimos espacamento para ficar alinhado com
   * outros momentos da academia.
   */
  if (N_CLIENTES - (N_EQUIPAMENTOS * N_CADEIRAS) == 0)
  {
    printf("     ");
  }
  printf("\\");
  for (i = 0; i < N_CADEIRAS; i++)
  {
    printf("______");
  }
  printf("__________________________/\n");

  /*****************************************************************************
   ********************** FIM DA IMPRESSÃO DA LINHA FINAL DA ACADEMIA  *********
   ****************************************************************************/

  return;
}

/* Thread dos equipamentos. */
void *f_equipamento(void *v)
{
  int id = *(int *)v;

  while (1)
  {

    sem_wait(&sem_cliente_entrou_equipamento[id]);
    

    /* Para o exercício nao ser imediato.*/
    sleep(1);

    
    sem_post(&sem_exercicio_feito[id]);


    sleep(random() % 3);
    
  }
  return NULL;
}

/* Thread dos clientes. */
void *f_cliente(void *v)
{
  int id = *(int *)v;

  sleep(random() % 3);

  sem_wait(&sem_estados);
  estadoC[id] = A;
  imprimeAcademia();
  sem_post(&sem_estados);

  for(int j = 0; j < N_EQUIPAMENTOS; j++){
    if (sem_wait(&sem_cadeira_livre[j][N_CADEIRAS - 1]) == 0)
      {
        sem_wait(&sem_estados);
          estadoC[id] = W;
          imprimeAcademia();
        sem_post(&sem_estados);
        for(int i = N_CADEIRAS-2; i >= 0; i--){
          sem_wait(&sem_cadeira_livre[j][i]);
          sem_wait(&sem_estados);
          estadoCadeiraCliente[j][i+1] = F;
          estadoCadeiraCliente[j][i] = B;
          clientesCadeira[j][i] = id;
          imprimeAcademia();
          sem_post(&sem_estados);
          sem_post(&sem_cadeira_livre[j][i+1]);
        }

        sleep(1);

        sem_wait(&sem_equipamento_livre[j]);
        sem_post(&sem_cadeira_livre[j][0]);
        
        sem_wait(&sem_estados);
          estadoC[id] = FC;
          estadoB[id] = FB;
          estadoCadeiraCliente[j][0] = F;
          clienteEquipamento[j] = id;
          imprimeAcademia();
        sem_post(&sem_estados);

        sem_post(&sem_cliente_entrou_equipamento[j]);


        sem_wait(&sem_exercicio_feito[j]);
        sem_post(&sem_equipamento_livre[j]);

        sem_wait(&sem_estados);
          estadoC[id] = W;
          estadoB[id] = S;
          imprimeAcademia();
        sem_post(&sem_estados);
      }
  }

  sem_wait(&sem_estados);
  estadoC[id] = E;
  imprimeAcademia();
  sem_post(&sem_estados);

  return NULL;
}

int main()
{
  pthread_t thr_clientes[N_CLIENTES], thr_equipamentos[N_EQUIPAMENTOS]; // vetor de threads dos equipamentos e clientes
  int i, j, id_cl[N_CLIENTES], id_equip[N_EQUIPAMENTOS];                // vetor de identificação dos equipamentos e clientes

  sem_init(&sem_estados, 0, 1); // inicia o semáforo dos estados

  for (i = 0; i < N_EQUIPAMENTOS; i++)
  {
    sem_init(&sem_cliente_entrou_equipamento[i], 0, 0);     // inicia o semáforo de escrever no visor
    sem_init(&sem_equipamento_livre[i], 0, 1); 
    sem_init(&sem_le_visor[i], 0, 0);          // inicia o semáforo de ler no visor
    sem_init(&sem_ultima_cadeira[i], 0, 1); // inicia o semáforo das cadeiras
    sem_init(&sem_cad_equipamento[i], 0, 1);
    sem_init(&sem_cliente_cadeira[i], 0, 0);
    sem_init(&sem_exercicio_feito[i], 0, 0);
    sem_wait(&sem_estados);
    estadoB[i] = S; // sleeping
    sem_post(&sem_estados);
    for(j = 0; j < N_CADEIRAS; j++)
    {
    sem_init(&sem_cadeira_livre[i][j], 0, 1);
    }
  }


  for (i = 0; i < N_CLIENTES; i++)
  {
    sem_wait(&sem_estados);
    estadoC[i] = I; // idle
    sem_post(&sem_estados);
  }
  for (i = 0; i < N_EQUIPAMENTOS; i++)
  {
    for (j = 0; j < N_CADEIRAS; j++)
    {
      sem_wait(&sem_estados);
      estadoCadeiraCliente[i][j] = F; // free
      sem_post(&sem_estados);
    }
  }

  for (i = 0; i < N_CLIENTES; i++)
  {
    id_cl[i] = i;
    pthread_create(&thr_clientes[i], NULL, f_cliente, (void *)&id_cl[i]);
  }

  for (i = 0; i < N_EQUIPAMENTOS; i++)
  {
    id_equip[i] = i;
    pthread_create(&thr_equipamentos[i], NULL, f_equipamento, (void *)&id_equip[i]);
  }

  for (i = 0; i < N_CLIENTES; i++)
    pthread_join(thr_clientes[i], NULL);

  return 0;
}