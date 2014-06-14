#if ! defined( JOGO_ )
#define JOGO_

/***************************************************************************
*  $MCI Módulo de definição: JOGO  gerenciador do jogo de damas
*
*  Arquivo gerado:              jogo.c
*  Letras identificadoras:      JOGO
*
*  Projeto: INF 1301 / 1628 Jogo de Damas
*  Gestor:  LES/DI/PUC-Rio
*  Autores: Gustavo Marques Martins (gmm), Hugo Pedrotti Grochau (hpg)
*
*  $HA Histórico de evolução:
*     Versão  Autor     Data        Observações
*     1       hpg/gmm   09/jun/2014 início desenvolvimento
*     2       hpg/gmm   13/jun/2014 início da lógica de movimento
*
*  $ED Descrição do módulo
*     Implementa a lógica e funcionamento geral do jogo de damas.
*
***************************************************************************/

#if defined( JOGO_OWN )
#define JOGO_EXT
#else
#define JOGO_EXT extern
#endif

/***** Declarações exportadas pelo módulo *****/

/***********************************************************************
*
*  $TC Tipo de dados: JOGO condições de retorno
*
*
*  $ED Descrição do tipo
*     Condições de retorno das funções do jogo
*
***********************************************************************/

typedef struct
{
    JOGO_CondRetOk,
    /* Concluiu corretamente */

	TAB_CondRetJogoAcabou,
	/* O jogo terminou */

    JOGO_CondRetJogadaInvalida,
    /* O jogador tentou fazer uma jogada invalida */

    JOGO_CondRetArquivoInvalido,
    /* Ocorreu erro a ler o arquivo */

    JOGO_CondRetArquivoCorrompido,
    /* Ocorreu erro a interpretar o arquivo */

    JOGO_CondRetJogoNaoInicializado
    /* O jogo não foi inicializado */

} JOGO_tpCondRet

/* Tipo referência para o jogo */
typedef JOGO_tpJogo *JOGO_tppJogo;

/***********************************************************************
*
*  $FC Função: JOGO  -Criar jogo
*
*  $ED Descrição da função
*     Cria uma nova instância de um jogo de damas, incializando todos
*	  os dados necessários com o estado de início de jogo.
*	
*  $EP Parâmetros
*     nomeJogador1 - o nome do primeiro jogador.
*	  nomeJogador2 - o nome do segundo jogador.
*
*  $FV Valor retornado
*     Se executou corretamente retorna o ponteiro para o jogo.
*	  Se ocorreu algum erro, a função retornará NULL
* 
***********************************************************************/

JOGO_tppJogo JOGO_CriarJogo (char *NomeJogador1, char *NomeJogador2);

/***********************************************************************
*
*  $FC Função: JOGO  -Destruir jogo
*
*  $ED Descrição da função
*     Deleta o jogo, limpando o espaço de memória alocado para ele
*     o tabuleiro, e os jogadores
*
*  $EP Parâmetros
*     jogo - referência para o tipo jogo a ser deletado.
*
***********************************************************************/

void JOGO_DestruirJogo (JOGO_tppJogo jogo);

/***********************************************************************
*
*  $FC Função: JOGO  -Preencher tabuleiro
*
*  $ED Descrição da função
*     Carrega o tabuleiro de um arquivo e insere as peças no tabuleiro
*
*  $EP Parâmetros
*     jogo - Ponteiro para o jogo a ser impresso.
*     arqTabuleiro - string contedo o path (diretorio/nome) do arquivo.
*
*  $FV Valor retornado
*     CondRetArquivoInvalido - Caso o arquivo não seja aberto ou não exista.
*     CondRetJogoNaoInicializado - O jogo ainda não foi inicializado
*     CondRetArquivoCorrompido - Dados invalidos
*     CondRetOk - Tabuleiro foi preenchido com sucesso
*
***********************************************************************/

JOGO_tpCondRet JOGO_PreencherTabuleiro (JOGO_tppJogo jogo, FILE *fp arqTabuleiro );

/***********************************************************************
*
*  $FC Função: JOGO  -Imprimir tabuleiro
*
*  $ED Descrição da função
*     Imprime uma representação visual do tabuleiro em um console do
*     Windows
*
*  $EP Parâmetros
*     jogo - Ponteiro para o jogo a ser impresso.
*
*  $FV Valor retornado
*     CondRetJogoNaoInicializado - O jogo ainda não foi inicializado
*     CondRetOk - Tabuleiro foi impresso com sucesso
*
***********************************************************************/

JOGO_tpCondRet JOGO_ImprimirTabuleiro (JOGO_tppJogo jogo);

#undef JOGO_EXT

/********** Fim do módulo de definição: PECA  Peça de tabuleiro de damas **********/

#else
#endif