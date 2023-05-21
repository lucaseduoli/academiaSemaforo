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

#define N_CLIENTES 10
#define N_EQUIPAMENTOS 3
#define N_CADEIRAS 3

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
int clientesEquipamento[N_EQUIPAMENTOS];

sem_t sem_cadeiras[N_EQUIPAMENTOS];
sem_t sem_cad_equipamento[N_EQUIPAMENTOS];
sem_t sem_exercicio_feito[N_EQUIPAMENTOS];
sem_t sem_cliente_cadeira[N_EQUIPAMENTOS];
sem_t sem_fila_mexendo[N_EQUIPAMENTOS];

sem_t sem_escreve_visor[N_EQUIPAMENTOS];
sem_t sem_le_visor[N_EQUIPAMENTOS];

sem_t sem_estados; /* Semaforo para troca de estados. */

int visor[N_EQUIPAMENTOS];

char equipamentos[3][4][21] = {
  {{"             /\\    "}, {"  __________/ /    "}, {" /__________\\/     "}, {"//         //      "},},
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
void imprimedAcademia()
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
  for (i = 0; i < qtosClientesArrived; i++)
  {
    printf("     ");
  }

  /* Faz a conta para imprimir o tí­tulo 'ACADEMIA' no meio. */

  if (qtosClientesArrived == 0)
  {
    printf("     ");
  }

  titulo = (4 + 6 * N_CADEIRAS +27) / 2;

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
  for (i = 0; i < qtosClientesArrived; i++)
    {
      printf("     ");
    }

    /* Se nao chegou clientes, imprimimos espacamento para ficar alinhado com
      * outros momentos da academia.
      */
    if (qtosClientesArrived == 0)
    {
      printf("     ");
    }
  printf(" ");
    for (i = 0; i < N_CADEIRAS; i++)
      {
        printf("______");
      }
      printf("__________________________\n");

      for (i = 0; i < qtosClientesArrived; i++)
    {
      printf("     ");
    }

    /* Se nao chegou clientes, imprimimos espacamento para ficar alinhado com
      * outros momentos da academia.
      */
    if (qtosClientesArrived == 0)
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
          for (i = 0; i < qtosClientesArrived; i++)
      {
        printf("     ");
      }

      /* Se nao chegou clientes, imprimimos espacamento para ficar alinhado com
       * outros momentos da academia.
       */
      if (qtosClientesArrived == 0)
      {
        printf("     ");
      }
      printf("|");
      for (i = 0; i < N_CADEIRAS; i++)
      {
        printf("      ");
      }
      printf("                          |\n");

    for (i = 0; i < qtosClientesArrived; i++)
    {
      printf(" ... ");
    }

    /* Se nao chegou clientes, imprimimos espacamento para ficar alinhado com
     * outros momentos da academia.
     */
    if (qtosClientesArrived == 0)
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

    for (i = 0; i < qtosClientesArrived; i++)
    {
      printf(" %2d  ", clientesArrived[i]);
    }

    /* Se nao chegou clientes, imprimimos espacamento para ficar alinhado com
     * outros momentos da academia.
     */
    if (qtosClientesArrived == 0)
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
      printf("       %2d  ", clientesEquipamento[k]);
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

    for (i = 0; i < qtosClientesArrived; i++)
    {
      printf("     ");
    }

    /* Se nao chegou clientes, imprimimos espacamento para ficar alinhado com
     * outros momentos da academia.
     */
    if (qtosClientesArrived == 0)
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
      for (i = 0; i < qtosClientesArrived; i++)
      {
        printf("     ");
      }

      /* Se nao chegou clientes, imprimimos espacamento para ficar alinhado com
       * outros momentos da academia.
       */
      if (qtosClientesArrived == 0)
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
    for(int j = 0; j < 4; j++){
      for (i = 0; i < qtosClientesArrived; i++)
    {
      printf("     ");
    }

    /* Se nao chegou clientes, imprimimos espacamento para ficar alinhado com
     * outros momentos da academia.
     */
    if (qtosClientesArrived == 0)
    {
      printf("     ");
    }

    printf("| ");

    for (i = N_CADEIRAS - 1; i >= 0; i--)
    {
      if(estadoCadeiraCliente[k][i] == B)
        printf(" %s ", pessoa[j]);
      else {
        printf("      ");
      }
    }

    printf("    ");

    printf(" %s ", equipamentos[k%3][j]);


      printf("| \n");
    }
    /*****************************************************************************
     ********************** FIM DA IMPRESSÃO DAS LINHAS DE ASCII DA ITERAÇÃO *****
     ****************************************************************************/
  }
    /*****************************************************************************
     ********************** IMPRESSÃO DA LINHA FINAL DA ACADEMIA  ****************
     ****************************************************************************/
    for (i = 0; i < qtosClientesArrived; i++)
      {
        printf("     ");
      }

      /* Se nao chegou clientes, imprimimos espacamento para ficar alinhado com
       * outros momentos da academia.
       */
      if (qtosClientesArrived == 0)
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
     ********************** fFIM DA IMPRESSÃO DA LINHA FINAL DA ACADEMIA  ********
     ****************************************************************************/



  return;
}

/* Thread dos equipamentos. */
void *f_equipamento(void *v)
{
  int id = *(int *)v;

  while (1)
  {
    sem_wait(&sem_escreve_visor[id]);
    visor[id] = id;
    sem_post(&sem_le_visor[id]);

    /* Para o exercício nao ser imediato.*/
    sleep(1);

    sem_wait(&sem_cliente_cadeira[id]);
    sem_post(&sem_exercicio_feito[id]);

    sleep(random() % 3);
  }
  return NULL;
}

/* Thread dos clientes. */
void *f_cliente(void *v)
{
  int id = *(int *)v;
  int i, j;
  int minha_cadeira, minhaCadeiraCliente, meuEquipamento;

  sleep(random() % 3);

  sem_wait(&sem_estados);
  /* Cliente chegou na academia. */
  estadoC[id] = A;

  /* Imprime que o cliente chegou na academia. */
  imprimedAcademia();
  sem_post(&sem_estados);

  for (j = 0; j < N_EQUIPAMENTOS; j++)
  {
    if (sem_trywait(&sem_cadeiras[j]) == 0)
    {
      /* Cliente entrou na academia e sentou em uma cadeira. */
      sem_wait(&sem_estados);
      estadoC[id] = W;

      /* Aloca uma cadeira para o cliente. */
      sem_wait(&sem_fila_mexendo[j]);
      for (i = 0; i < N_CADEIRAS; i++)
      {
        if (estadoCadeiraCliente[j][i] == F)
        {
          estadoCadeiraCliente[j][i] = B;
          clientesCadeira[j][i] = id;

          minhaCadeiraCliente = i;
          meuEquipamento = j;

          break;
        }
      }
      sem_post(&sem_fila_mexendo[j]);

      /* Imprime que o cliente esta esperando na academia. */
      imprimedAcademia();
      sem_post(&sem_estados);

      /* Cliente espera o visor mostrar um equipamento livre. */
      while (minhaCadeiraCliente != 0)
      {
        if (estadoCadeiraCliente[j][minhaCadeiraCliente - 1] == F)
        {
          sem_wait(&sem_estados);
          sem_wait(&sem_fila_mexendo[j]);
          estadoCadeiraCliente[j][minhaCadeiraCliente] = F;
          estadoCadeiraCliente[j][minhaCadeiraCliente - 1] = B;
          clientesCadeira[j][minhaCadeiraCliente - 1] = id;
          minhaCadeiraCliente--;
          sem_post(&sem_fila_mexendo[j]);
          imprimedAcademia();
          sem_post(&sem_estados);
          
        }
      }
      sem_wait(&sem_le_visor[j]);
      minha_cadeira = visor[j];
      /* Permite que um outro equipamento escreva no visor. */
      sem_post(&sem_escreve_visor[j]);
      /* Espera cadeira do equipamento ficar livre para sentar. */
      sem_wait(&sem_cad_equipamento[minha_cadeira]);

      sem_post(&sem_cliente_cadeira[minha_cadeira]);

      sem_wait(&sem_estados);

      /* Libera a cadeira que estava sentado para um novo cliente que chegar. */
      sem_post(&sem_cadeiras[j]);

      /* Altera os estados para fazer o exercício. */
      estadoC[id] = FC;
      estadoB[minha_cadeira] = FB;

      /* Guarda que o cliente esta na cadeira do equipamento indicada por
       * minha_cadeira.
       */
      clientesEquipamento[minha_cadeira] = id;

      estadoCadeiraCliente[meuEquipamento][minhaCadeiraCliente] = F;

      /* Imprime que o cliente vai fazer o exercício. */
      imprimedAcademia();
      sem_post(&sem_estados);

      /* Termino de alterar e imprimir a academia. */

      sem_wait(&sem_exercicio_feito[minha_cadeira]);

      sem_wait(&sem_estados);
      /* Cliente sai da academia. */
      estadoC[id] = E;
      estadoB[minha_cadeira] = S;

      /* Imprime que o cliente saiu da academia. */
      imprimedAcademia();

      sem_post(&sem_cad_equipamento[minha_cadeira]);

      /* Altera o estado pois o cliente vai embora. */
      estadoC[id] = L;
      sem_post(&sem_estados);
    }
    else
    {
      /* Cliente desistiu de fazer os exercícios. */
      sem_wait(&sem_estados);
      estadoC[id] = L;
      sem_post(&sem_estados);
    }
  }

  return NULL;
}

int main()
{
  pthread_t thr_clientes[N_CLIENTES], thr_equipamentos[N_EQUIPAMENTOS]; // vetor de threads dos equipamentos e clientes
  int i, j, id_cl[N_CLIENTES], id_equip[N_EQUIPAMENTOS];                // vetor de identificação dos equipamentos e clientes

  sem_init(&sem_estados, 0, 1); // inicia o semáforo dos estados

  for (i = 0; i < N_EQUIPAMENTOS; i++)
  {
    sem_init(&sem_escreve_visor[i], 0, 1);     // inicia o semáforo de escrever no visor
    sem_init(&sem_le_visor[i], 0, 0);          // inicia o semáforo de ler no visor
    sem_init(&sem_cadeiras[i], 0, N_CADEIRAS); // inicia o semáforo das cadeiras
    sem_init(&sem_cad_equipamento[i], 0, 1);
    sem_init(&sem_cliente_cadeira[i], 0, 0);
    sem_init(&sem_exercicio_feito[i], 0, 0);
    sem_init(&sem_fila_mexendo[i], 0, 1);

    sem_wait(&sem_estados);
    estadoB[i] = S; // sleeping
    sem_post(&sem_estados);
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